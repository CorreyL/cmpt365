#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include <string>

using namespace cv;
class VideoHelper
{
public:
	static double getFrameCount(CvCapture* capture);
	static void playVideo(std::string videoName, int size);
};

#pragma once
