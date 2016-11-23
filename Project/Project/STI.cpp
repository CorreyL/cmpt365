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

using namespace cv;
using namespace std;

STI::STI(int size, int frames)
{
	stiColMat = Mat(size, frames, CV_8UC3);
	stiRowMat = Mat(frames, size, CV_8UC3); //printed type using function
	//side note: this may be confusing as C reverses row and column ordering.
	videoRes = size;
	numFrames = frames;
}

STI::~STI()
{

}

void STI::setStiMatrix(string videoName, int size)
//sets the number of columns according to frame count
{
	int enlarge = 5;
	VideoCapture cap(videoName); // open the video file
	if (!cap.isOpened())  // check if we succeeded
		FAIL("Could not open video capture.");
	namedWindow("Video", 1);
	int currentFrame;
	currentFrame = 0;
	cout << "CurrentFrame " << currentFrame << endl;
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
	showColImage(enlarge);
	showRowImage(enlarge);

	createFrameHistogram(stiColMat);
}

void STI::showColImage(int enlarge)
{
	Mat M;
	resize(stiColMat, M, Size(numFrames * enlarge, videoRes * enlarge));
	namedWindow("StiColMatrix", WINDOW_AUTOSIZE);
	imshow("StiColMatrix", M);
}

int STI::chromNormalization(float chrom) {
	if(chrom >= 0 && chrom < 0.144){
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

void STI::createFrameHistogram(Mat image) {
	// Mat hist = Mat(7, 7, CV_32F, double(0)); //create empty histogram 7x7
	int hist[7][7] = {0};
	float rchrom = 0;
	float gchrom = 0;
	if (DEBUG)
		cout << "createHist image input: rows = " << image.rows << " cols = " << image.cols << endl;
	for (int i = 0; i < image.rows; i++) { //make sure that this is correct
		for (int j = 0; j < image.cols; j++) {
			//default returned is BGR
			Vec3b intensity = image.at<cv::Vec3b>(i,j);
			//get R,G,B values from image
			double r = (double)intensity.val[2];
			double b = (double)intensity.val[1];
			double g = (double)intensity.val[0];
	
			
			rchrom = (r / (r + b + g));
			gchrom = (g / (r + b + g));

			int rHist = chromNormalization(rchrom);
			int gHist = chromNormalization(gchrom);

			hist[rHist][gHist] = hist[rHist][gHist] + 1;
		}
	}
	// Prints out the histogram
	for (int l = 0; l < 7; l++) {
		for (int m = 0; m < 7; m++) {
			cout << hist[l][m] << " ";
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

void STI::makeHistogramSTI(std::string videoName, int size, int frameCount) {
	VideoCapture cap(videoName);
	if (!cap.isOpened())  // check if we succeeded
		throw runtime_error("Could not open video capture.");

	// Retrieves the first frame;
	Mat original;
	cap.read(original);

	// Since the first frame has nothing to be compared to, we store it in capPrevFrame right away
	Mat capPrevFrame;
	resize(original, capPrevFrame, Size(size, size));

	// The STI image we are going to return
	Mat final_image = Mat(capPrevFrame.cols, frameCount, CV_8UC1, Scalar(0));

	// Let the rows be the R direction & columns be the G direction for (r,g) = (r,g)/(r+g+b)
	float prevFrame[7][7] = { { 0 } };
	float currFrame[7][7] = { { 0 } };

	float rchrom;
	float gchrom;

	int numberOfFramesProcessed = 1; // Starts at 1 since we already have 1 frame in capPrevFrame
	Mat capCurrFrame;

	int finalImageRowTracker = 0; // Used to keep track of where in the STI we need to store the final pixel value in
	while (numberOfFramesProcessed < frameCount) {
		cap.read(original); // Getting a new frame from video
		resize(original, capCurrFrame, Size(size, size)); // Resizing it and putting it into capCurrFrame
		for (int i = 0; i < capCurrFrame.cols; i++) {
			for (int j = 0; j < capCurrFrame.rows; j++) {
				// Obtaining the Histogram for capPrevFrame
				Vec3b intensity = capPrevFrame.at<cv::Vec3b>(i, j); // Retrieve the pixel information at the ith and jth entry
				double r = (double)intensity.val[2];
				double b = (double)intensity.val[1];
				double g = (double)intensity.val[0];

				double divideByZeroTest = r + b + g;
				if (divideByZeroTest == 0) {
					rchrom = 0;
					gchrom = 0;
				}
				else {
					rchrom = (r / (r + b + g));
					gchrom = (g / (r + b + g));
				}

				int rHist = chromNormalization(rchrom);
				int gHist = chromNormalization(gchrom);

				prevFrame[rHist][gHist] = prevFrame[rHist][gHist] + 1;

				// Obtaining the Histogram for capCurrFrame
				intensity = capCurrFrame.at<cv::Vec3b>(i, j); // Retrieve the pixel information at the ith and jth entry
				r = (double)intensity.val[2];
				b = (double)intensity.val[1];
				g = (double)intensity.val[0];

				divideByZeroTest = r + b + g;
				if (divideByZeroTest == 0) {
					rchrom = 0;
					gchrom = 0;
				}
				else {
					rchrom = (r / (r + b + g));
					gchrom = (g / (r + b + g));
				}

				rHist = chromNormalization(rchrom);
				gHist = chromNormalization(gchrom);

				currFrame[rHist][gHist] = currFrame[rHist][gHist] + 1;
			}

			// The Histogram information for the same column in each frame has been captured, now we need to run a comparison
			// First we need to normalize prevFrame/currFrame such that the sum of every element in a histogram == 1
			float prevSum = 0;
			float currSum = 0;
			for (int x = 0; x < 7; x++) {
				for (int y = 0; y < 7; y++) {
					prevSum = prevSum + prevFrame[x][y];
					currSum = currSum + currFrame[x][y];
				}
			}

			// With the sum obtained, we need to divide every entry in prevFrame/currFrame by their respective sums
			for (int x = 0; x < 7; x++) {
				for (int y = 0; y < 7; y++) {
					prevFrame[x][y] = prevFrame[x][y] / prevSum;
					currFrame[x][y] = currFrame[x][y] / currSum;
				}
			}
			// Now if we were to sum every element of prevFrame/currFrame, it would equal 1

			// With the sum of prevFrame & currFrame now equal to one, we run the Histogram Intersection formula
			float HistIntersectSum = 0;
			float prevFrameVal = 0;
			float currFrameVal = 0;
			for (int x = 0; x < 7; x++) {
				for (int y = 0; y < 7; y++) {
					prevFrameVal = prevFrame[x][y];
					currFrameVal = currFrame[x][y];

					// Find the min of the above two values, and add it to the sum of HistIntersectSum
					if (currFrameVal > prevFrameVal) {
						HistIntersectSum = HistIntersectSum + prevFrameVal;
					}
					else {
						HistIntersectSum = HistIntersectSum + currFrameVal;
					}
				}
			}
			// HistIntersectSum should roughly equal 1 if the column current frame and previous frame are from the same scene

			final_image.at<uchar>(Point(finalImageRowTracker, i)) = HistIntersectSum * 255;
			// Reset prevFrame and currFrame Histograms
			for (int x = 0; x < 7; x++) {
				for (int y = 0; y < 7; y++) {
					prevFrame[x][y] = 0;
					currFrame[x][y] = 0;
				}
			}
		}

		// Make the current frame the previous frame now
		capCurrFrame.copyTo(capPrevFrame);
		finalImageRowTracker++; // This increments it so that in the next iteration, we update the next column of the STI
		numberOfFramesProcessed++;
	}
	imshow("Test", final_image);
}