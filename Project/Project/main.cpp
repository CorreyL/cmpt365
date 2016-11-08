#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//Plays video
	VideoCapture cap("VideoFiles/WipeVert1_480p.avi"); // open the video file
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	namedWindow("Video", 1);
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera        
		if (!frame.empty()) 
		{
			Mat resized;
			resize(frame, resized, Size(32, 32));
			imshow("Video", resized);
		}
		if (waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}