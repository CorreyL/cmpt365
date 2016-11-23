#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

#include "VideoHelper.h"
#include "STI.h"

#define DEBUG 1

using namespace cv;
using namespace std;
/*
int chromNormalization(float chrom) {
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

Mat makeHistogramSTI(string videoName, int frameCount) {
	VideoCapture cap(videoName);
	if (!cap.isOpened())  // check if we succeeded
		throw runtime_error("Could not open video capture.");
	int size = 500;

	// Retrieves the first frame;
	Mat original;
	cap.read(original);

	// Since the first frame has nothing to be compared to, we store it in capPrevFrame right away
	Mat capPrevFrame;
	resize(original, capPrevFrame, Size(size, size));

	// The STI image we are going to return
	Mat final_image = Mat(capPrevFrame.cols, frameCount, CV_8UC1, Scalar(0));

	// Let the rows be the R direction & columns be the G direction for (r,g) = (r,g)/(r+g+b)
	float prevFrame[7][7] = {{0}};
	float currFrame[7][7] = {{0}};

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
	return final_image;
}
*/

int main(int argc, char** argv)
{
	//set constants for main
	string videoFile = "VideoFiles/WipeVert1_480p.avi";
	int size = 300; //set matrix resize to 32

	CvCapture* capture = cvCaptureFromAVI(videoFile.c_str());
	int frameCount = (int)VideoHelper::getFrameCount(capture);
	if (DEBUG)
		cout << "FrameCount = " << frameCount << endl;
	
	//play video file
	VideoHelper::playVideo(videoFile, size);
	
	//create new sti
	STI* sti = new STI(size, frameCount);
	//sti->printColMatrix();
	sti->setStiMatrix(videoFile, size);
	sti->makeHistogramSTI(videoFile, size, frameCount);
	delete sti;
	cvWaitKey(0);

	return EXIT_SUCCESS;
}