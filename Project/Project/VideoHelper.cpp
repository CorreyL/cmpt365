#include "VideoHelper.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

double VideoHelper::frameCount(CvCapture* capture) {
	double frameCount;
	frameCount = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	return frameCount;
}
