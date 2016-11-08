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
	void setStiColMatrix(std::string videoName, int size );
	void printColMatrix();
	void printRowMatrix();
	std::string type2str(int type);
	void showColImage(int enlarge);
	void showRowImage(int enlarge);

private: 
	Mat stiColMat;
	Mat stiRowMat;
	int videoRes;
	int numFrames;
};
