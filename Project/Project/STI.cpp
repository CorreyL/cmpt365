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
	else if (chrom >= 0.864 && chrom < 1.000) {
		return 6;
	}
	else {
		throw runtime_error("An invalid value was passed in. Should be between 0 and 1.");
	}
}

void STI::createFrameHistogram(Mat image) {
	Mat hist = Mat(7, 7, CV_32F, double(0)); //create empty histogram 7x7
	 
	if (DEBUG)
		cout << "createHist image input: rows = " << image.rows << " cols = " << image.cols << endl;
	for (int i = 0; i < image.rows; i++) { //make sure that this is correct
		for (int j = 0; j < image.cols; j++) {
			//default returned is BGR
			Vec3b intensity = image.at<cv::Vec3b>(i,j);
			//get R,G,B values from image
			int r = (int)intensity.val[2];
			int b = (int)intensity.val[1];
			int g = (int)intensity.val[0];

			float rchrom = (r / (r + b + g));
			float gchrom = (g / (r + b + g));
		}
		int rHist = chromNormalization(rchrom);
		int gHist = chromNormalization(gchrom);
		
		// hist[rHist][gHist] = hist[rHist][gHist] + 1; // Not sure if I can access a Mat array like this?
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