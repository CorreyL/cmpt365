#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp""

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	VideoCapture cap("VideoFiles/WipeVert1_480p.avi"); // open the default camera
						 //Video Capture cap(path_to_video); // open the video file
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	namedWindow("Video", 1);
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera        
		if (!frame.empty())
			imshow("Video", frame);
		if (waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}