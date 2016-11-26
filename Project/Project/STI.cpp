#include "STI.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include <stdio.h>
#include <iostream>
#include <string>

#define DEBUG 1
#define FAIL(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }
#define MIN(a,b) ((a<b) ? a : b)
using namespace cv;
using namespace std;

STI::STI(int size, int frames)
{
	stiColMat = Mat(size, frames, CV_8UC3);
	stiRowMat = Mat(frames, size, CV_8UC3); //printed type using function
	stiColHist = Mat(size, frames, CV_64F); //MAXX: maybe can rid of these
	stiRowHist = Mat(frames, size, CV_64F);
	//side note: this may be confusing as C reverses row and column ordering.
	videoRes = size;
	numFrames = frames;
}

STI::~STI() {}

void STI::setStiMatrix(string videoName, int size)
//sets the number of columns according to frame count
{
	int enlarge = 3;
	VideoCapture cap(videoName); // open the video file
	if (!cap.isOpened())  // check if we succeeded
		FAIL("Could not open video capture.");
	namedWindow("Video", 1);
	int currentFrame;
	currentFrame = 0;
	int centerCol = size / 2;
	int centerRow = size / 2;
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from video   

		if (!frame.empty())
		{
			//resize video to size x size
			Mat resized;
			resize(frame, resized, Size(size, size));
			resized.col(centerCol).copyTo(stiColMat.col(currentFrame));	
			resized.row(centerRow).copyTo(stiRowMat.row(currentFrame));
			currentFrame++;
			//imshow("Video", resized);
		}
		if (waitKey(10) >= 0) break;
	}
	cvDestroyWindow("Video");
	cap.release();
	//showColImage(enlarge);
	//showRowImage(enlarge);
}

void STI::showColImage(int enlarge)
{
	Mat M;
	resize(stiColMat, M, Size(numFrames * enlarge, videoRes * enlarge));
	namedWindow("StiColMatrix", WINDOW_AUTOSIZE);
	imshow("StiColMatrix", M);
}


void STI::createStiHistogram(std::string videoName, int size, int frameCount) {
	//Open the video as we did in STI
	VideoCapture cap(videoName);
	if (!cap.isOpened())
		FAIL("Couldnot open video capture.");
	namedWindow("Video", 1); //Not sure id this is necessary
	int currentFrame = 0; //set current frame to 0

	//Set up Mat for final image and frames in use
	Mat finalImage = Mat(size, frameCount, CV_32F, Scalar(0));
	
	Mat prevFrameHist, currFrameHist;

	//to hold r andg chrom values
	float rchrom, gchrom;

	//capture PrevFrame and current Frame
	Mat capPrevFrame, capCurrFrame;
	namedWindow("PrevFrame", 1);
	namedWindow("CurrFrame", 1);
	cap >> capPrevFrame;
	resize(capPrevFrame, capPrevFrame, Size(size, size));
	prevFrameHist = getFrameHist(capPrevFrame);
	for (;;) { //this is done while the video loops
		Mat frame;
		cap >> frame;
		
		/*if (currentFrame == (frameCount - 1))
			break; //since we are grabbing 2 frames at once*/
		if (!capCurrFrame.empty())
		{
			resize(frame, capCurrFrame, Size(size, size));
			imshow("PrevFrame", capPrevFrame);
			imshow("CurrFrame", capCurrFrame);
			//copy the current frame to the previous frame
			capCurrFrame.copyTo(capPrevFrame);
		}
		currentFrame++;
		if (waitKey(10) >= 0) break;
	}
	cvDestroyWindow("Video");
	cap.release(); 
}

Mat STI::getFrameHist(Mat frame) {
	//init histogram
	Mat hist = Mat(7, 7, CV_32F, float(0));
	Mat rg;
	float rchrom, gchrom;
	if (DEBUG)
		cout << "Frame rows, cols = (" << frame.rows << "," << frame.cols << ")" << endl;
	for (int i = 0; i < frame.rows; i++) {
		for (int j = 0; j < frame.cols; j++) {
			Vec3b intensity = frame.at<cv::Vec3b>(i, j);
			rg = getChromValues(intensity);
			int r = chromNormalization(rg.at<float>(0));
			int g = chromNormalization(rg.at<float>(1));
			hist.at<float>(r,g) = hist.at<float>(r,g) + 1.0f;
		}
	}
	//normalize histogram
	float pixelSum = 1.0f / (frame.rows * frame.cols); //equiv to dividing by pixelsum
	Mat normalizedHist = pixelSum * hist;
	if (DEBUG)
		printHist(normalizedHist);
	return normalizedHist;
}

Mat STI::getChromValues(Vec3b intensity) 
{
	float r = (float)intensity.val[2];
	float g = (float)intensity.val[1];
	float b = (float)intensity.val[0];
	Mat rg = divByZero(r, g, b);
	return rg;
}

void STI::makeHistogramSTI(std::string videoName, int size, int frameCount){
	VideoCapture cap(videoName);
	if (!cap.isOpened())  // check if we succeeded
		throw runtime_error("Could not open video capture.");

	// Retrieves the first frame;
	Mat original, capPrevFrame, capCurrFrame;
	//cap.read(original);
	cap >> original;
	// Since the first frame has nothing to be compared to, we store it in capPrevFrame right away
	resize(original, capPrevFrame, Size(size, size));

	// The STI image we are going to return
	Mat final_image = Mat(capPrevFrame.cols, frameCount, CV_32F, Scalar(0));

	// Let the rows be the R direction & columns be the G direction for (r,g) = (r,g)/(r+g+b)
	float prevFrameHist[7][7] = { { 0 } };
	float currFrameHist[7][7] = { { 0 } };

	float rchrom, gchrom;

	int numberOfFramesProcessed = 1; // Starts at 1 since we already have 1 frame in capPrevFrame

	int finalImageRowTracker = 0; // Used to keep track of where in the STI we need to store the final pixel value in
	namedWindow("CapPrevFrame", WINDOW_AUTOSIZE);
	imshow("CapPrevFrame", capPrevFrame);
	cvWaitKey(20);
	//make the initial hist
	for (int i = 0; i < capPrevFrame.rows; i++) {
		for (int j = 0; j < capPrevFrame.cols; j++) {
			// Obtaining the Histogram for capPrevFrame
			Vec3b intensity = capPrevFrame.at<cv::Vec3b>(i, j); // Retrieve the pixel information at the ith and jth entry
			cout << "Vec3b intensity ( " <<i<<","<<j<<") " << intensity << endl;
		
			Mat rg = getChromValues(intensity);

			int rHist = chromNormalization(rg.at<float>(0));
			int gHist = chromNormalization(rg.at<float>(1));

			prevFrameHist[rHist][gHist] = prevFrameHist[rHist][gHist] + 1;
		}
	}

	while (numberOfFramesProcessed < frameCount) {
		cap.read(original); // Getting a new frame from video
		resize(original, capCurrFrame, Size(size, size)); // Resizing it and putting it into capCurrFrame
		
		for (int i = 0; i < capCurrFrame.rows; i++) {
			for (int j = 0; j < capCurrFrame.cols; j++) {
				// Obtaining the Histogram for capPrevFrame


				// Obtaining the Histogram for capCurrFrame
				//Put these in function since they are repetitive.
				Vec3b intensity = capCurrFrame.at<cv::Vec3b>(i, j); // Retrieve the pixel information at the ith and jth entry
				float r = (float)intensity.val[2];
				float b = (float)intensity.val[1];
				float g = (float)intensity.val[0];
				Mat rg = divByZero(r, g, b);
				int rHist = chromNormalization(rg.at<float>(0));
				int gHist = chromNormalization(rg.at<float>(1));
				currFrameHist[rHist][gHist] = currFrameHist[rHist][gHist] + 1;

			
			}

			// The Histogram information for the same column in each frame has been captured, now we need to run a comparison
			// First we need to normalize prevFrameHist/currFrameHist such that the sum of every element in a histogram == 1
			float prevSum = 0;
			float currSum = 0;
			//no for loop, always going to be how many rows for a col sti, eg. 1 x rows (for a col)
			int pixelSum = capCurrFrame.rows;
			// With the sum obtained, we need to divide every entry in prevFrameHist/currFrameHist by their respective sums
			for (int x = 0; x < 7; x++) {
				for (int y = 0; y < 7; y++) {
					prevFrameHist[x][y] = prevFrameHist[x][y] / (float)pixelSum;
					currFrameHist[x][y] = currFrameHist[x][y] / (float)pixelSum;
				}
			}
			// Now if we were to sum every element of prevFrameHist/currFrameHist, it would equal 1

			// With the sum of prevFrameHist & currFrameHist now equal to one, we run the Histogram Intersection formula
			float HistIntersectSum = 0;
			float prevFrameVal = 0;
			float currFrameVal = 0;
			for (int x = 0; x < 7; x++) {
				for (int y = 0; y < 7; y++) {
					prevFrameVal = prevFrameHist[x][y];
					currFrameVal = currFrameHist[x][y];
					HistIntersectSum += MIN(prevFrameHist[x][y], currFrameHist[x][y]);
				}
			}
			// HistIntersectSum should roughly equal 1 if the column current frame and previous frame are from the same scene

			final_image.at<float>(Point(finalImageRowTracker, i)) = HistIntersectSum;
			
			// Reset prevFrameHist and currFrame Histograms
			for (int x = 0; x < 7; x++) {
				for (int y = 0; y < 7; y++) {
					prevFrameHist[x][y] = currFrameHist[x][y];
					currFrameHist[x][y] = 0;
				}
			}
		}

		// Make the current frame the previous frame now
		capCurrFrame.copyTo(capPrevFrame);
		finalImageRowTracker++; // This increments it so that in the next iteration, we update the next column of the STI
		numberOfFramesProcessed++;
	}
	if (DEBUG) {
		cout << "Number of rows = number of columns in video frame, number of cols = frameCount" << endl;
		cout << "final_image: rows = " << final_image.rows << " ; cols = " << final_image.cols << endl;
		cout << "Cols in vid fram =  " << capPrevFrame.cols << "; Framecount = " << frameCount << endl;
	}
	namedWindow("STIHistImage", WINDOW_AUTOSIZE);
	imshow("STIHistImage", final_image);
	cvWaitKey(20);
}

Mat STI::divByZero(float r, float g, float b) {
	//Returns a 2 entry Mat for (r,g) value, where rg(0) = rchrom, and rg(1) = gchrom
	float divideByZeroTest = r + b + g; //can you just conpare in the if?
	Mat rg = Mat(1, 2, CV_32F); //create 1d matrix 
	if (divideByZeroTest == 0) {
		rg.at<float>(0) = 0; //rchrom
		rg.at<float>(1) = 0; //gchrom
	}
	else {
		rg.at<float>(0) = (r / (r + b + g));
		rg.at<float>(1) = (g / (r + b + g));
	}
	return rg;
}

int STI::chromNormalization(float chrom) {
	if (chrom >= 0 && chrom < 0.144) {
		return 0;
	}
	else if (chrom >= 0.144 && chrom < 0.288) {
		return 1;
	}
	else if (chrom >= 0.288 && chrom < 0.432) {
		return 2;
	}
	else if (chrom >= 0.432 && chrom < 0.576) {
		return 3;
	}
	else if (chrom >= 0.576 && chrom < 0.720) {
		return 4;
	}
	else if (chrom >= 0.720 && chrom < 0.864) {
		return 5;
	}
	else if (chrom >= 0.864 && chrom <= 1.000) {
		return 6;
	}
	else {
		throw runtime_error("An invalid value was passed in. Should be between 0 and 1.");
	}
}

void STI::printHist(Mat hist) {
	for (int i = 0; i < hist.rows; i++) {
		for (int j = 0; j < hist.cols; j++) {
			cout << hist.at<float>(i, j) << "   ";
		}
		cout << endl;
	}
}

void STI::showRowImage(int enlarge)
{
	Mat M;
	resize(stiRowMat, M, Size(numFrames * enlarge, videoRes * enlarge));
	namedWindow("StiRowMatrix", WINDOW_AUTOSIZE);
	imshow("StiRowMatrix", M);
}

string STI::type2str(int type) {
	/* use like so:
	string ty = type2str(resized.type());
	printf("Matrix: %s %dx%d \n", ty.c_str(), resized.cols, resized.rows);
	*/
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}