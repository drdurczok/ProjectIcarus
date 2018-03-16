#ifndef AUDIO_H
#define AUDIO_H

#include <alsa/asoundlib.h>
#include <festival/festival.h>

class Audio
{
	public:
		Audio();

		void recordAudio(int len);
		void playback(std::string file);
		void festival(std::string speakString);

	private:

		long loops;
	    int rc;
	    int size;
	    snd_pcm_t *handle;
   	    snd_pcm_hw_params_t *params;
	    unsigned int val;
	    int dir;
	    snd_pcm_uframes_t frames;
	    char *buffer;

};

#endif
