#include "ImageHelper.h"


#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"


using namespace cv;
using namespace std;

 Mat ImageHelper::convertImage(Mat image)
{
	//scale image to 64 x 64 pixels
	cv::Size size(64, 64);
	Mat dest;
	//Mat resizedImage(size);
	cv::resize(image, dest, size, 0, 0);
	//convert each pixel to 4 bit values
	for (int row = 0; row < dest.rows; row++) {
		for (int col = 0; col < dest.cols; col++) {
			dest.at<uchar>(col, row) = ((floor(dest.at<uchar>(col, row)))/16);
		}
	}
	return dest;
}

 void ImageHelper::printColumn(Mat col) {
	 for (int i = 0; i < col.rows; i++) {
		 for (int j = 0; j < col.cols; j++) {
			 uchar val;
			 val = col.at<uchar>(i, j);
			 cout << (int)val << endl;
		 }
	 }
 }
