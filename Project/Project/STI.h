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
	void setStiMatrix(std::string videoName, int size);
	std::string type2str(int type);
	void showColImage(int enlarge);
	void showRowImage(int enlarge);
	int chromNormalization(float chrom);
	void createStiHistogram(std::string videoName, int size, int framCount);
	void createStiHistogramThres(std::string videoName, int size, int frameCount);
	void printHist(Mat hist);
	void displayImage(Mat image, int enlarge, std::string name);

	Mat getFrameHist(Mat frame);
	Mat divByZero(float r, float g, float b);
	Mat getChromValues(Vec3b intensity);

private:
	Mat stiColMat;
	Mat stiRowMat;
	Mat stiColHist;
	Mat stiRowHist;
	int videoRes;
	int numFrames;
};
