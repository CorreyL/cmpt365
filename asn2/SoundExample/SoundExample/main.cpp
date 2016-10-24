#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mixer.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "GetFile.h"
#include "ImageHelper.h"
#include <list>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include "audio.h"

using namespace cv;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Plays a sequence of tones according to strings entered by the user
// argc: ignored
// argv: ignored
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{

	//VideoCapture vc = ImageHelper::getVideo();

	//Mat frame, gray;

	//while (true){

	// Get the image from the video file
		//vc.read(frame);

		// Stop if there are no more frames in the video
		//if (frame.empty()) break;

		//cvtColor(frame, gray, CV_BGR2GRAY);

		//gray = ImageHelper::convertImage(gray);
	Mat image_src;
	const string filename = "exx.png";
	image_src = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	Mat image = ImageHelper::convertImage(image_src);
	namedWindow("image", CV_WINDOW_AUTOSIZE);
	imshow("image", image);

	//takes the column of an image
	Mat mat1col = Mat::zeros(1, 64, CV_32F);
	//ImageHelper::printColumn(image.col(32));
	image.col(0).copyTo(mat1col);
	// namedWindow("mat1col", CV_WINDOW_AUTOSIZE);
	// imshow("mat1col", mat1col);
	Audio::Open();
	float freq[64];
	freq[31] = 440.0f; //centering around middle A
	int i;
	for (i = 32; i < 64; ++i) {
		freq[i] = (freq[i - 1] * pow(2, (1.0 / 12.0)));
	}
	for (i = 30; i > -1; --i) {
		freq[i] = (freq[i + 1] * pow(2, (-1.0 / 12.0)));
	}
	//Allocate channels for mixing
	Mix_AllocateChannels(64);
	//volume from 0 to 128 ***THIS IS JUST REFERENCE*****
	//Mix_Volume(1, MIX_MAX_VOLUME/2); //assign half vol to channel1
	//printf*"Average Volume is %d\n",Mix_volume(-1,-1); //prints average volume

	for (int sure = 0; sure < 64; sure++) {
		uchar intensity = image.at<uchar>(sure,0);
		printf("Intensity is %i at row %i", (int)intensity, sure);
	/*	if ((int)intensity == 0) { //Here is to set the volume on every channel
			printf("Mat1col at %i is 0", sure);
			Mix_Volume(sure, 0);
		}
		else {
			Mix_Volume(sure, MIX_MAX_VOLUME / (int)intensity);
		}*/
		float frequency = freq[sure];
		printf("Playing at %f Hz\n", frequency);
		// Allocate a buffer for 0.1 seconds
		const size_t len = 44100 / 10;//Audio::GetFrequency() / 10;
		float buf[len];
		//float bufTest[64];
		float **bufTest = new float*[64];
		for (int tempo = 0; tempo < 64; ++tempo) {
			bufTest[tempo] = new float[len];
		}
		// Write a sine wave
		for (size_t i = 0; i < len / 2; i++)
		{
			// i * 2 is the frame number (2 samples per frame, one for each channel)
			//bufTest is 2d Buffer
			// Left
			buf[i * 2 + 0] = sinf(i * 2 * frequency * 2 * float(M_PI) / Audio::GetFrequency()) * 0.5f;
			bufTest[sure][i * 2 + 0] = sinf(i * 2 * frequency * 2 * float(M_PI) / Audio::GetFrequency()) * 0.5f;
			// Right
			buf[i * 2 + 1] = sinf(i * 2 * frequency * 2 * float(M_PI) / Audio::GetFrequency()) * 0.5f;
			bufTest[sure][i * 2 + 1] = sinf(i * 2 * frequency * 2 * float(M_PI) / Audio::GetFrequency()) * 0.5f;
		}

		// Play the sound
	//	Audio::Play(buf, len);
		Audio::PlayAll(bufTest, len);

			// Wait for it to stop playing
		Audio::WaitForSilence();
	}
	
	waitKey();
	
	Audio::Close();
	return EXIT_SUCCESS;
}

