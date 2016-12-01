#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

#include "VideoHelper.h"
#include "STI.h"

#define DEBUG 0

using namespace cv;
using namespace std;


int main(int argc, char** argv)
{
	//set constants for main
	string videoFile = "VideoFiles/WipeVert1_480p.avi";
	//string videoFile = "VideoFiles/WhiteToBlack.mp4";
	/*cout << "How big would you like the video displayed to be?" << endl;
	int size = -1;
	bool invalid_size = true;
	while (invalid_size) {
		cin >> size;
		if (size < 1) {
			cout << "That is an invalid size input. Please input a number greater than 0." << endl;
		}
		else {
			invalid_size = false;
		}
	}
	*/
	CvCapture* capture = cvCaptureFromAVI(videoFile.c_str());
	int frameCount = (int)VideoHelper::getFrameCount(capture);
	int size = 32;
	//create new sti
	STI* sti = new STI(size, frameCount);

	cout << "Beginning Section 1.1" << endl;
	sti->setStiMatrix(videoFile, size);
	cout << endl;

	cout << "Beginning Section 1.2" << endl;
	sti->createStiHistogram(videoFile, size, frameCount);
	cout << "Press Enter to exit the program." << endl;

	delete sti;
	cvWaitKey(0);

	return EXIT_SUCCESS;
}