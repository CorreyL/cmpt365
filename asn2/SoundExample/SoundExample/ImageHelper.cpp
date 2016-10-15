#include "ImageHelper.h"


#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;



Mat ImageHelper::convertImage(Mat image)
{
	//scale image to 64 x 64 pixels
	Size size(64, 64);
	Mat dest;
	cv::resize(image, dest, size, 0, 0);
	return dest;
}
