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
	ImageHelper::printColumn(image.col(32));
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

	for (int sure = 0; sure < 64; ++sure) {
		float frequency = freq[sure];// *10 + 200.0f;
		printf("Playing at %f Hz\n", frequency);
		// Allocate a buffer for 0.1 seconds
		const size_t len = 44100 / 10;//Audio::GetFrequency() / 10;
		float buf[len];

		// Write a sine wave
		for (size_t i = 0; i < len / 2; i++)
		{
			// i * 2 is the frame number (2 samples per frame, one for each channel)

			// Left
			buf[i * 2 + 0] = sinf(i * 2 * frequency * 2 * float(M_PI) / Audio::GetFrequency()) * 0.5f;

			// Right
			buf[i * 2 + 1] = sinf(i * 2 * frequency * 2 * float(M_PI) / Audio::GetFrequency()) * 0.5f;
		}

		// Play the sound
		Audio::Play(buf, len);

		// Wait for it to stop playing
		Audio::WaitForSilence();
		
	}
	waitKey();
		//assign ea. note to a channel in the audio. Recall that if the pixel value is black, the note will not 
	const size_t len = Audio::GetFrequency() / 16; // each chord plays 0.0625 s
	float * buf = new float[len];


		//Write the sine wav
	for (int col = 0; col < 64; col++) { // Column
		memset(buf, 0, sizeof(buf));
		for (int row = 0; row < 64; row++) { // Row
			int freqAcc = 63 - row;
			float tempFreq = freq[freqAcc];
				printf("Playing at %f Hz\n", tempFreq);
				for (int m = 0; m < len / 2; m++) {
					// Left //changed gray to image
					buf[m * 2 + 0] = (float)image.at<uchar>(row, col)* sinf(2 * float(M_PI) * 2 * freq[freqAcc] / Audio::GetFrequency()) * 0.5f;

					// Right
					buf[m * 2 + 1] = (float)image.at<uchar>(row, col)*sinf(2 *float(M_PI) * 2 *freq[freqAcc] / Audio::GetFrequency()) * 0.5f;
					//
				}
			}
			Audio::Play(buf, len);
			cout << "PLAYED AUDIO" << endl;
			Audio::WaitForSilence();
		}
			waitKey();
		// Sleep for 30 milliseconds. Note: This function is the only method in 
		// HighGUI that can fetch and handle events,  so it needs to be called 
		// periodically for normal event processing unless HighGUI is used within 
		// an environment that takes care of event processing.
		int c = waitKey(30);

		// Quit if any key is pressed
	//	if (c != -1) break;
	//} //end while
	
	Audio::Close();
	return EXIT_SUCCESS;
}
/*
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mixer.h>

#include <list>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "audio.h"

// Prints a message and exits with EXIT_FAILURE.
#define FAIL(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }


int main(int argc, char* argv[])
{
	float freq[64];
	freq[31] = 440.0f; //centering around middle A
	int i;
	for (i = 32; i < 64; ++i) {
		freq[i] = (freq[i - 1] * pow(2, (1.0 / 12.0)));
	}
	for (i = 30; i > -1; --i) {
		freq[i] = (freq[i + 1] * pow(2, (-1.0 / 12.0)));
	}
	Audio::Open();

	int input_character;

	// Retrieve a character, or stop if Ctrl+D is entered (Ctrl+Z in Windows)
	printf("Type a string to play it: ");
	int sure = 10;
	while ((input_character = getchar()) != EOF)
	{
		if (input_character == '\n')
		{
			printf("Type a string to play it: ");
			continue;
		}

		// Use the character to create a frequency.
		//float frequency = input_character * 10 + 200.0f;
		float frequency = freq[sure];// *10 + 200.0f;
		printf("Playing at %f Hz\n", frequency);
		printf("input char is %i", (int)input_character);
		// Allocate a buffer for 0.1 seconds
		const size_t len = 44100 / 10;//Audio::GetFrequency() / 10;
		float buf[len];

		// Write a sine wave
		for (size_t i = 0; i < len / 2; i++)
		{
			// i * 2 is the frame number (2 samples per frame, one for each channel)

			// Left
			buf[i * 2 + 0] = sinf(i * 2 * frequency * 2 * float(M_PI) / Audio::GetFrequency()) * 0.5f;

			// Right
			buf[i * 2 + 1] = sinf(i * 2 * frequency * 2 * float(M_PI) / Audio::GetFrequency()) * 0.5f;
		}
		sure = sure + 3;
		// Play the sound
		Audio::Play(buf, len);

		// Wait for it to stop playing
		Audio::WaitForSilence();
	}

	Audio::Close();

	return EXIT_SUCCESS;
}*/
