#include "../include/Audio.h"


Audio::Audio()
{
  int heap_size = 210000;  // default scheme heap size
  int load_init_files = 1; // we want the festival init files loaded
  festival_initialize(load_init_files,heap_size);
}

void Audio::recordAudio(int len) 
{
  std::ofstream outfile ("audio.raw",std::ofstream::binary);

  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) 
  {
    fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
    exit(1);
  }



  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);



  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

  /* Single channel (monoaural) 1 and Two channels (stereo) 2*/
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* bits/second sampling rate */
  val = 8000;
  snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

  /* Set period size to x frames. */
  frames = 256;
  snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) 
  {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
    exit(1);
  }



  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  size = frames * 4; /* 2 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  loops = len / val;

  std::cout << "Recording Audio..." << std::endl;

  while (loops > 0) {
    loops--;
    rc = snd_pcm_readi(handle, buffer, frames);
    if (rc == -EPIPE) 
    {
      /* EPIPE means overrun */
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      fprintf(stderr, "error from read: %s\n", snd_strerror(rc));
    } else if (rc != (int)frames) {
      fprintf(stderr, "short read, read %d frames\n", rc);
    }

   outfile.write(buffer, size);      
  }

  outfile.close();

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);
}

void Audio::playback(std::string file)
{
  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) 
  {
    fprintf(stderr,
        "unable to open pcm device: %s\n",
        snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

  /* Single channel (monoaural) 1 and Two channels (stereo) 2*/
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = 44100;
  snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

  /* Set period size to 32 frames. */
  frames = 1024;
  snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0)
  {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  size = frames * 4; /* 2 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);
  
  /* Open file to be read from */
  std::ifstream infile (file.c_str(), std::ifstream::binary);
  /* Move to beginning of file */
  infile.seekg (0, std::ios::beg);

  while (!infile.eof()) 
  {
    infile.read(buffer, size);
   
    rc = snd_pcm_writei(handle, buffer, frames);
    if (rc == -EPIPE) {
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
    }  else if (rc != (int)frames) {
      fprintf(stderr, "short write, write %d frames\n", rc);
    }
  }

  infile.close();

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);
}

void Audio::festival(std::string speakString)
{
    // Say some text;
    festival_say_text(speakString.c_str());
}