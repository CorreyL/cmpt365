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
	//convert each pixel to 4 bit values
	for (int row = 0; row < dest.rows; row++) {
		for (int col = 0; col < dest.cols; col++) {
			dest.at<uchar>(col, row) = (floor(dest.at<uchar>(col, row) * 16)) / 16;
		}
	}
	return dest;
}
