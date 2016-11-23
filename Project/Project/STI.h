#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"

#include <string>

using namespace cv;

class STI
{
public:
	STI(int cols, int rows); //constructor
	~STI(); //destructor
	void setStiMatrix(std::string videoName, int size );
	std::string type2str(int type);
	void showColImage(int enlarge);
	void showRowImage(int enlarge);
	void createFrameHistogram(Mat image);
	int chromNormalization(float chrom);
	void makeHistogramSTI(std::string videoName, int size, int frameCount);

private: 
	Mat stiColMat;
	Mat stiRowMat;
	int videoRes;
	int numFrames;
};
