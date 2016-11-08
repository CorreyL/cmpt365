#include "VideoHelper.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include <string>

#define FAIL(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

using namespace cv;
using namespace std;

double VideoHelper::getFrameCount(CvCapture* capture) {
	double frameCount;
	frameCount = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	return frameCount;
}

void VideoHelper::playVideo(string videoName, int size) 
{
	VideoCapture cap(videoName); // open the video file
	if (!cap.isOpened())  // check if we succeeded
		FAIL("Could not open video capture.");
	namedWindow("Video", 1);
	
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from video        
		if (!frame.empty())
		{
			//resize video to size x size
			Mat resized;
			resize(frame, resized, Size(size, size));
			imshow("Video", resized);
		}
		if (waitKey(10) >= 0) break;
	}
	cvDestroyWindow("Video");
	cap.release();
	// the camera will be deinitialized automatically in VideoCapture destructor
}
