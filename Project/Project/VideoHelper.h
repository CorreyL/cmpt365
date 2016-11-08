#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
class VideoHelper
{
public:
	double frameCount(CvCapture* capture);
};

#pragma once
