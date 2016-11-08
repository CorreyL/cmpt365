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
// argc: ignored
// argv: ignored
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	cout << "Please select a file to load:" << endl;
	char videoFilename[1024];
	GetFile(videoFilename, 1024);
	if (videoFilename[0] == '\0')
	{
		fprintf(stderr, "Please select a file\n");
		return EXIT_FAILURE;
	}

	VideoCapture vc = VideoCapture(videoFilename);

	Mat frame;
	Mat gray;

	//create frequency array for each pixel
	double freq[64];
	freq[32] = 440.0; //centering around middle A
	int i;
	for (i = 33; i < 64; i++) {
		freq[i] = freq[i - 1] * pow(2, (1.0 / 12.0));
	}
	for (i = 31; i > -1; i--) {
		freq[i] = freq[i + 1] * pow(2, (-1.0 / 12));
	}
	
	while (true)
	{
		// Get the image from the video file
		vc.read(frame);

		// Stop if there are no more frames in the video
		if (frame.empty()) break;

		cvtColor(frame, gray, CV_BGR2GRAY);

		gray = ImageHelper::convertImage(gray);

		Audio::Open();
		int N = 500;
		// int samplingFreq = 8000; //sampling freq is also set in Audio::GetFrequency

		// assign ea. note to a channel in the audio. Recall that if the pixel value is black, the note will not play
		const size_t len = Audio::GetFrequency() / 16; // each chord plays 0.0625 s
		float * buf = new float[len];

		// Building tt like it does in the MATLAB sample code
		// tt = (1:N) initializes an array with the values from 1 to N, where tt[0] = 1, tt[1] = 2, ... tt[N-1] = N
		// tt = (1:N)/Fs divides every entry by Fs; Since Fs = 8000, tt[0] = 1/8000, tt[1] = 2/8000, ... tt[N-1] = N/8000
		float * tt = new float[N];
		for (int j = 0; j < N; j++) {
			tt[j] = (j + 1) / 8000;
		}
		// Note that we do not use this tt array to multiply in our sinf() function like it does in the MATLAB code, as this produces no sound if we do.

		imshow("image", gray);
		//Write the sine wav
		for (int col = 0; col < 64; col++) { // Column
			memset(buf, 0, sizeof(buf));
			for (int row = 0; row < 64; row++) { // Row
				int freqAcc = 63 - row;
				for (int m = 0; m < len / 2; m++) {
					// Left
					buf[m * 2 + 0] = (float)gray.at<uchar>(row, col) * (sinf(2 * float(M_PI) * freq[freqAcc] / Audio::GetFrequency()));

					// Right
					buf[m * 2 + 1] = (float)gray.at<uchar>(row, col) * (sinf(2 * float(M_PI) * freq[freqAcc] / Audio::GetFrequency()));

				}
			}
			Audio::Play(buf, len);
			Audio::WaitForSilence();
		}
		cout << '\a';
		// Sleep for 30 milliseconds. Note: This function is the only method in 
		// HighGUI that can fetch and handle events,  so it needs to be called 
		// periodically for normal event processing unless HighGUI is used within 
		// an environment that takes care of event processing.
		int c = waitKey(30);

		// Quit if any key is pressed
		if (c != -1) break;
	}
	
	Audio::Close();
	return EXIT_SUCCESS;
}
