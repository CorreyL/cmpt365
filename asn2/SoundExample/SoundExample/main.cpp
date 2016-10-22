#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mixer.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
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
   
	Mat image_src;
	const string filename = "phone.jpg";
	image_src = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	Mat image = ImageHelper::convertImage(image_src);
	namedWindow("image", CV_WINDOW_AUTOSIZE);
	imshow("image", image);

	//takes the column of an image
	Mat mat1col = Mat::zeros(1, 64, CV_32F);
	cout << "Printing image.col(0)" << endl;
	ImageHelper::printColumn(image.col(0));
	image.col(0).copyTo(mat1col);
	namedWindow("mat1col", CV_WINDOW_AUTOSIZE);
	imshow("mat1col", mat1col);

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
	waitKey();
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

	//assign ea. note to a channel in the audio. Recall that if the pixel value is black, the note will not play
	const size_t len = Audio::GetFrequency() / N; // each chord plays 0.0625 s
	float * buf = new float[len]; 
	//Write the sine wav

	// m * 64 is the frame number (64 samples per frame, one for each channel)
	for (size_t m = 0; i < len / 64; m++) { //64 samples per frame, one for each channel
		for (int n = 0; n < 64; n++) {
			buf[m * 64 + n] = sinf(m * 2 * freq[n] * M_PI  / Audio::GetFrequency()) * 0.5f; //not sure what 0.5f does
			//buf[m * 64 + n + 1] = sinf(m * 2 * freq[n] * M_PI / Audio::GetFrequency()) * 0.5f; //not sure what 0.5f does

		}
	}
	// Play the sound
	Audio::Play(buf, len);
	printf("Just played the chord");
	// Wait for it to stop playing
	Audio::WaitForSilence();
	Audio::Close();
	waitKey();
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
