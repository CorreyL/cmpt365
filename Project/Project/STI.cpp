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
	//Mat prevFrameHist, currFrameHist;

	//to hold r andg chrom values
	float rchrom, gchrom;

	//capture PrevFrame and current Frame
	Mat capPrevFrame, capCurrFrame;
	namedWindow("PrevFrame", 1);
	namedWindow("CurrFrame", 1);
	cap >> capPrevFrame;
	resize(capPrevFrame, capPrevFrame, Size(size, size));
	
	for (;;) { //this is done while the video loops
		Mat frame;
		cap >> frame;
		
		/*if (currentFrame == (frameCount - 1))
			break; //since we are grabbing 2 frames at once*/
		if (!frame.empty())
		{
			resize(frame, capCurrFrame, Size(size, size));
			imshow("PrevFrame", capPrevFrame);
			imshow("CurrFrame", capCurrFrame);
			int rowTracker = 0; // Keeps track of which row entry we need to place the value of histCompTotal into
			for (int i = 0; i < capPrevFrame.cols; i++) {
				cout << "Enter prev FrameHist" << endl;
				Mat prevFrameHist = getFrameHist(capPrevFrame.col(i));
				Mat currFrameHist = getFrameHist(capCurrFrame.col(i));
				
				float histCompTotal = 0;
				for (int x = 0; x < currFrameHist.rows; x++) {
					for (int y = 0; y < currFrameHist.cols; y++) {
						histCompTotal = histCompTotal + min(prevFrameHist.at<float>(x, y), currFrameHist.at<float>(x, y));
					}
				}
				finalImage.at<float>(rowTracker, currentFrame) = histCompTotal * 255;
				imshow("Final Image", finalImage);
				rowTracker++; // We've placed this column comparison into a row, need to put the next comparison into the next row
				/*
				string test;
				cout << "histCompTotal is: " << histCompTotal << endl;
				cin >> test;
				*/
			}
			
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
	// if (DEBUG)
		// printHist(normalizedHist);
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