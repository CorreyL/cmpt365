#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mixer.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "GetFile.h"

#include <list>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include "audio.h"

// Prints a message and exits with EXIT_FAILURE.
#define FAIL(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

// Declare this again to satisfy the linker
Audio::ChunkList Audio::chunks_played;

///////////////////////////////////////////////////////////////////////////////
void Audio::Open()
{
	// Mix_Init(0) will do nothing. If you wish to play audio from files,
	// however, then you will need to send the correct argument to Mix_Init.
	Mix_Init(0);

	int rc = Mix_OpenAudio(
		44100, // Frequency
		AUDIO_F32,      // Format. Note: several formats are not portable.
		2,              // Channels , nothing to do with audio channels
		4096);          // Chunk size. Should not be too small.
	if (rc == -1) FAIL("Could not open audio.");
	
	int frequency;
	Uint16 format;
	int channels;
	Mix_QuerySpec(&frequency, &format, &channels);
	if (format != AUDIO_F32) FAIL("Audio format is not correct.")
}
///////////////////////////////////////////////////////////////////////////////
void Audio::Close()
{
	Mix_CloseAudio();

	Audio::ChunkList::iterator chunk_i = chunks_played.begin();
	while (chunk_i != chunks_played.end())
	{
		Mix_FreeChunk((*chunk_i).first);
		chunk_i++;
	}

	Mix_Quit();
}
///////////////////////////////////////////////////////////////////////////////
void Audio::Play(const float * buf, size_t len)
{
	// Delete any chunks that are no longer playing
	Audio::ChunkList::iterator chunk_i = chunks_played.begin();
	while (chunk_i != chunks_played.end())
	{
		// Test the channel on which we played the original chunk
		// to see if it is still playing the same chunk
		if (Mix_GetChunk((*chunk_i).second) != (*chunk_i).first)
		{
			// If the channel was not playing the same chunk, then the
			// chunk is not being played, so delete it
			Mix_FreeChunk(chunk_i->first);
			chunks_played.erase(chunk_i++);
		}
		else
		{
			chunk_i++;
		}
	}

	float* bufcpy = (float*)SDL_malloc(sizeof(*bufcpy) * len);
	memcpy(bufcpy, buf, len * sizeof(*buf));

	// Create a chunk from the data
	Mix_Chunk *chunk = Mix_QuickLoad_RAW((Uint8*)bufcpy, len * sizeof(*buf));

	// Set chunk->allocated to non-zero to cause SDL to call
	// SQL_free on the chunk's data buffer when Mix_FreeChunk is called
	chunk->allocated = true;

	// Play the chunk
	int rc = Mix_PlayChannel(-1, chunk, 0);
	if (rc == -1)
	{
		FAIL("Could not play chunk.");
	}
	else
	{
		// Add the chunk and the channel it was played on to the list of
		// chunks we played
		chunks_played.push_back(std::pair<Mix_Chunk*, int>(chunk, rc));
	}
}
///////////////////////////////////////////////////////////////////////////////
void Audio::PlayAll(const float * const* buf, size_t len)
{
	Mix_Chunk* chunkArr[64];
	// Delete any chunks that are no longer playing
	Audio::ChunkList::iterator chunk_i = chunks_played.begin();
	while (chunk_i != chunks_played.end())
	{
		// Test the channel on which we played the original chunk
		// to see if it is still playing the same chunk
		if (Mix_GetChunk((*chunk_i).second) != (*chunk_i).first)
		{
			// If the channel was not playing the same chunk, then the
			// chunk is not being played, so delete it
			Mix_FreeChunk(chunk_i->first);
			chunks_played.erase(chunk_i++);
		}
		else
		{
			chunk_i++;
		}
	}
	if (Mix_GroupChannels(0, 63, 1) != 64) { //tried to group channels, then TRIED to look for method to play the group
		printf("ERROR BAD CHANNELS");
	}
	int temp;
	for ( temp = 0; temp < 63; temp++) { //make chunk for every pixel
		float* bufcpy = (float*)SDL_malloc(sizeof(*bufcpy) * len);
		memcpy(bufcpy, buf[temp], len * sizeof(*buf));
		// Create a chunk from the data
		Mix_Chunk *chunk = Mix_QuickLoad_RAW((Uint8*)bufcpy, len * sizeof(*buf));
		
		// Set chunk->allocated to non-zero to cause SDL to call
		// SQL_free on the chunk's data buffer when Mix_FreeChunk is called
		chunk->allocated = true;
		chunkArr[temp] = chunk;
	}
	// Play the chunk
	//for (temp = 0; temp < 63; temp++) { //to iterate over all pixels
		Mix_Chunk* tChunk = chunkArr[0]; //0 was temp
		int rc = Mix_PlayChannel(0, tChunk, 0);
		if (rc == -1)
		{
			FAIL("Could not play chunk.");
		}
		else
		{
			// Add the chunk and the channel it was played on to the list of
			// chunks we played
			chunks_played.push_back(std::pair<Mix_Chunk*, int>(tChunk, rc));
		}
	//}

}
///////////////////////////////////////////////////////////////////////////////
void Audio::WaitForSilence()
{
	while (Mix_Playing(-1)) SDL_Delay(10);
}