#pragma once
#include "Data.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable : 4996);

namespace Audio
{
	void play_Audio(const char* file_path)
	{
		SDL_AudioSpec wav_spec;
		Uint32 wav_length;
		Uint8 *wav_buffer;
		
		//load the wav file
		SDL_LoadWAV(file_path, &wav_spec, &wav_buffer, &wav_length);

		//play audio
		SDL_AudioDeviceID device1 = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
		int success = SDL_QueueAudio(device1, wav_buffer, wav_length);
		SDL_PauseAudioDevice(device1, 0);
	}
}