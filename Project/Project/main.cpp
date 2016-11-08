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


int main(int argc, char** argv)
{
	//set constants for main
	string videoFile = "VideoFiles/WipeVert1_480p.avi";
	int size = 32; //set matrix resize to 32

	CvCapture* capture = cvCaptureFromAVI(videoFile.c_str());
	int frameCount = (int)VideoHelper::getFrameCount(capture);
	if (DEBUG)
		cout << "FrameCount = " << frameCount << endl;
	
	//play video file
	VideoHelper::playVideo(videoFile, size);
	
	//create new sti
	STI* sti = new STI(size, frameCount);
	//sti->printColMatrix();
	sti->setStiColMatrix(videoFile, size);
	delete sti;
	cvWaitKey(0);

	return EXIT_SUCCESS;
}