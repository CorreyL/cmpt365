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
		//int samplingFreq = 8000; //sampling freq is also set in Audio::GetFrequency

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

		//assign ea. note to a channel in the audio. Recall that if the pixel value is black, the note will not 
		const size_t len = Audio::GetFrequency() / 16; // each chord plays 0.0625 s
		float * buf = new float[len];

		// Building tt like it does in the MATLAB sample code
		// tt = (1:N) initializes an array with the values from 1 to N, where tt[0] = 1, tt[1] = 2, ... tt[N-1] = N
		// tt = (1:N)/Fs divides every entry by Fs; Since Fs = 8000, tt[0] = 1/8000, tt[1] = 2/8000, ... tt[N-1] = N/8000
		float * tt = new float[N];
		for (int j = 0; j < N; j++) {
			tt[j] = (j + 1) / 8000;
		}

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

					// I want to multiply both of these by image.at<uchar>(row, col), since in the MATLAB code
					// ss = sin(2*pi*freq(m)*tt)
					// signal = signal + im(row, col) * ss;
					// In our case, im(row, col) should be image.at<uchar>(row, col)
					//
					// However, if you try to cout << image.at<uchar>(0, 0) << endl
					// The output is a strange symbol, not the numbers they should be
				}
			}
			Audio::Play(buf, len);
			Audio::WaitForSilence();
		}

		// Sleep for 30 milliseconds. Note: This function is the only method in 
		// HighGUI that can fetch and handle events,  so it needs to be called 
		// periodically for normal event processing unless HighGUI is used within 
		// an environment that takes care of event processing.
		int c = waitKey(30);

		// Quit if any key is pressed
		if (c != -1) break;
	}
	
	Mat image_src;
	const string filename = "phone.jpg";
	image_src = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	Mat image = ImageHelper::convertImage(image_src);
	namedWindow("image", CV_WINDOW_AUTOSIZE);
	imshow("image", image);

	//takes the column of an image
	Mat mat1col = Mat::zeros(1, 64, CV_32F);
	cout << "Printing image.col(0)" << endl;
	ImageHelper::printColumn(image.col(32));
	image.col(0).copyTo(mat1col);
	// namedWindow("mat1col", CV_WINDOW_AUTOSIZE);
	// imshow("mat1col", mat1col);
	

	//create an array of frequencies;
/*	int Fs = 8000;
	float freq[64];
	int i;
	freq[32] = 440; //centering frequencies around A
	for (i = 33; i < 64; i++) {
		freq[i] = freq[i - 1] * pow(2, (1 / 12));
	}
	for (i = 31; i > -1; i--) {
		freq[i] = freq[i + 1] * pow(2, (-1 / 12));
	}

	//Set the sample rate where 64 columns will take 4s
	int N = 500;
	//create array for the sound signals
	float signal[64];
	for (i = 0; i < mat1col.rows; i++) {
		//get highest frequency for top, since it is backwards
		int tempFreq = 63 - i + 1;
		float ss = sin(2 *M_PI*freq[i]);
		signal[i] = mat1col.at<uchar>(1,i) * ss;
	}
	;*/
	Audio::Close();
	return EXIT_SUCCESS;
}


	//START OLD CODE
	/* Audio::Open();
	


	int input_character;

    // Retrieve a character, or stop if Ctrl+D is entered (Ctrl+Z in Windows)
    printf("Type a string to play it: ");
    while ((input_character = getchar()) != EOF)
    {
        if (input_character == '\n')
        {
            printf("Type a string to play it: ");
            continue;
        }

        // Use the character to create a frequency.
        float frequency = input_character * 10 + 200.0f;
        printf("Playing at %f Hz\n", frequency);

        // Allocate a buffer for 0.1 seconds
        const size_t len = Audio::GetFrequency() / 10;
        float* buf = new float[len];
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

    Audio::Close();
	*/
