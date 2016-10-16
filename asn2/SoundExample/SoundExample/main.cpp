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
	//iterate through matrix and change the pixel values to 4 bit

	/*for (int row = 0; row < image.rows; row ++) {
		for (int col = 0; col < image.cols;col++) {
			///Scalar temp = floor(image.at<uchar>(col, row) *16);
			image.at<uchar>(col, row) = (floor(image.at<uchar>(col, row) * 16))/16;
			//*p++;
		}
	}*/

	//takes the column of an image
	Mat mat1col = Mat::zeros(1, 64, CV_32F);
	image.col(0).copyTo(mat1col);
	//mat1col = mat1col * 16;
	namedWindow("mat1col", CV_WINDOW_AUTOSIZE);
	imshow("mat1col", mat1col);
	waitKey();


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
    return EXIT_SUCCESS;
}
