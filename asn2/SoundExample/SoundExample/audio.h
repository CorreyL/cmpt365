#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mixer.h>

#include <list>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <math.h>

// A class for some basic audio functions
class Audio
{
public:
	// Opens SDL Mixer
	static void Open();

	// Closes SDL Mixer
	static void Close();

	// Plays the given audio, immediately. Does not block.
	// buf: Samples for two channels, interleaved, at the frequency given by GetFrequency.
	// len: The number of samples in channel one plus the number of samples in 
	//      channel two
	static void Play(const float * buf, size_t len);

	// Sleeps until there is no more audio playing
	static void WaitForSilence();

	// Returns the frequency to use for playing samples
	static int GetFrequency()
	{
		return 44100; //was 44100
	}

	static int GetChannels() { //added by Maxx
		return 1; //for number of channels needed to play chord
	}
private:

	// A list which keeps track of which chunks have been played, and which channel they
	// were played on. We need to manage the chunks ourselves because otherwise SDL won't
	// automatically delete them.
	typedef std::list<std::pair<Mix_Chunk*, int /* channel number */ > > ChunkList;
	static ChunkList chunks_played;
};