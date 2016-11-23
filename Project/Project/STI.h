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
<<<<<<< HEAD
	int chromNormalization(float chrom);
	void makeHistogramSTI(std::string videoName, int size, int frameCount);
=======
	int chromNormalization(double chrom);
	double histogramIntersect(Mat previous, Mat current);
	void createStiHistogram(std::string videoName, int size, int frameCount);
	void printHist(Mat hist);
>>>>>>> 88ad8909ff653ac7f1596c6654e43cac35ea5ffa

private: 
	Mat stiColMat;
	Mat stiRowMat;
	Mat stiColHist;
	Mat stiRowHist;
	int videoRes;
	int numFrames;
};
