#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <string>
#include <cmath>
#include <cstdio>
#define _USE_MATH_DEFINES
			
void error_output(const char* error_message, int error_code) {
	fprintf(stderr, "%s (%s)\n", error_message, snd_strerror(error_code));
	exit(1);
}

snd_pcm_t* initialise(const char* device, snd_pcm_t *playback_handle, snd_pcm_hw_params_t *hw_params) {
	int err;
	unsigned int rate_near=44100;	
	if (err=snd_pcm_open (&playback_handle, device, SND_PCM_STREAM_PLAYBACK, 0)<0) {
		char *message;	
		sprintf(message, "cannot open audio device %s", device);
		error_output(message, err);
	}

	if(err=snd_pcm_hw_params_malloc(&hw_params)<0) {error_output("cannot allocate hardware parameter structure", err);}
	if(err=snd_pcm_hw_params_any(playback_handle, hw_params)<0) {error_output("cannot initialize hardware parameter structure",err);}
	if(err=snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)<0){error_output("cannot set access type",err);}
	if(err=snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)<0){error_output("cannot set sample format",err);}
	if(err=snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &rate_near, 0)<0){error_output("cannot set sample rate",err);}
	if(err=snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2)<0){error_output("cannot set channel count",err);}
	if(err=snd_pcm_hw_params(playback_handle, hw_params)<0){error_output("cannot set parameters",err);}
	snd_pcm_hw_params_free(hw_params);
	if(err=snd_pcm_prepare(playback_handle)<0){error_output("cannot prepare audio interface for use",err);}
	return playback_handle;	
}

int main (int argc, char *argv[]) {
	int err;
	unsigned char *buf;
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;
	const char *device="default";
	if (argc>=2) {device=argv[1];}

	printf("initialising... ");
	playback_handle=initialise(device, playback_handle, hw_params);
	printf("done\n");

	int num_channels=2;
	int frame_size=2<<10;
	int num_frames=128;	
	buf=(unsigned char *)malloc(num_channels*frame_size*num_frames*2);

	for(int i=0; i<num_frames; i++) {
		for(int j=0; j<frame_size; ++j) {
			for(int k=0; k<num_channels; k++) {
				int volume=128;
				//short s=(j%(128/(k+1)))*volume;
				short s=5000*(cos(j*M_PI*4/frame_size));
				if (i==0) printf("%d %d %d %d\n", i,j,k,s);
				buf[2*(k+num_channels*(j+frame_size*i))+0]=s%256;
				buf[2*(k+num_channels*(j+frame_size*i))+1]=s>>8;
			}
		}
	}

	for (int i = 0; i < 250; ++i) {
		if(err=snd_pcm_writei(playback_handle, buf, num_frames)!=num_frames){error_output("write to audio interface failed",err);}
	}

	snd_pcm_close(playback_handle);
	exit (0);
}
