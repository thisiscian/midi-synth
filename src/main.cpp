#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <string>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <ncurses.h>
#define _USE_MATH_DEFINES
using namespace std;	
class SoundInfo {
	public:
		unsigned int channels;
		unsigned int *sample_rate;
		SoundInfo() {
			channels=2;
			sample_rate=(unsigned int*) malloc(sizeof(unsigned int));
			*sample_rate=44100;
		}
};
		
void error_output(const char* error_message, int error_code) {
	fprintf(stderr, "%s (%s)\n", error_message, snd_strerror(error_code));
	exit(1);
}

enum TONE_TYPE {
	SIN
};

int return_test(int x){
	return x;
}

void generate_tone(snd_pcm_t *handle, int volume, int note, int pitch, float length, SoundInfo sound_info=SoundInfo(), int type=SIN ) {
	unsigned char* data;
	unsigned int number_of_frames;
	float notes[12]={16.35,17.32,18.35,19.45,20.60,21.83,23.12,24.50,25.96,27.50,29.14,30.87};
	float frequency=notes[note]*(2<<pitch);
	number_of_frames=(int) (length*(*(sound_info.sample_rate))/sound_info.channels);
	data=(unsigned char *)malloc(number_of_frames*2*sound_info.channels);
	for(int i=0; i<number_of_frames; ++i) {
		short s=volume*(cos(M_PI/2+2*M_PI*i*frequency*length/(number_of_frames)));
		for(int j=0; j<sound_info.channels; ++j) {
			data[2*(i*sound_info.channels+j)+0]=s%256;
			data[2*(i*sound_info.channels+j)+1]=s>>8;
		}
	}
	int err;
	err=snd_pcm_writei(handle, data, number_of_frames);
	if(err<0) {
		err = snd_pcm_recover(handle, err, 0);
		if(err=snd_pcm_prepare(handle)<0){error_output("cannot prepare audio interface for use",err);}
		snd_pcm_drop(handle);
	}
	if(err<0){error_output("write to audio interface failed",err);}
	return;
}



snd_pcm_t* initialise(const char* device="default" ) {
	int err;
	SoundInfo si;
	unsigned int rate_near=44100;	
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;
	if (err=snd_pcm_open(&playback_handle, device, SND_PCM_STREAM_PLAYBACK, 0)<0) {
		char *message;	
		sprintf(message, "cannot open audio device %s", device);
		error_output(message, err);
	}

	if(err=snd_pcm_hw_params_malloc(&hw_params)<0) {error_output("cannot allocate hardware parameter structure", err);}
	if(err=snd_pcm_hw_params_any(playback_handle, hw_params)<0) {error_output("cannot initialize hardware parameter structure",err);}
	if(err=snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)<0){error_output("cannot set access type",err);}
	if(err=snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)<0){error_output("cannot set sample format",err);}
	if(err=snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, si.sample_rate, 0)<0){error_output("cannot set sample rate",err);}
	if(err=snd_pcm_hw_params_set_channels(playback_handle, hw_params, si.channels)<0){error_output("cannot set channel count",err);}
	if(err=snd_pcm_hw_params(playback_handle, hw_params)<0){error_output("cannot set parameters",err);}
	snd_pcm_hw_params_free(hw_params);
	if(err=snd_pcm_prepare(playback_handle)<0){error_output("cannot prepare audio interface for use",err);}
	return playback_handle;	
}

int main (int argc, char *argv[]) {
	int err;
	unsigned char *buf;
	snd_pcm_t *playback_handle;

	printf("#initialising... ");
	if(argc>=2) {
		playback_handle=initialise(argv[1]);
	} else {
		playback_handle=initialise();
	}
	printf("done\n");

	bool continue_playing=false;
	int osc=0;
	char note;
	int pitch=4;
	int note_num=-1;
	initscr();
	erase();
	refresh();
	cbreak();
	noecho();
	while(!continue_playing) {
		note=getch();
		refresh();
		erase();
		refresh();
		switch(note) {
			case 'a': note_num=0; printw("playing C\n"); break;
			case 'w': note_num=1; printw("playing C#\n"); break;
			case 's': note_num=2; printw("playing D\n"); break;
			case 'e': note_num=3; printw("playing Eb\n"); break;
			case 'd': note_num=4; printw("playing E\n"); break;
			case 'f': note_num=5; printw("playing F\n"); break;
			case 't': note_num=6; printw("playing F#\n"); break;
			case 'g': note_num=7; printw("playing G\n"); break;
			case 'y': note_num=8; printw("playing G#\n"); break;
			case 'h': note_num=9; printw("playing A\n"); break;
			case 'u': note_num=10; printw("playing Bb\n"); break;
			case 'j': note_num=11; printw("playing B\n"); break;
			case 'q':
				snd_pcm_close(playback_handle);
				refresh();
				endwin();
				exit(0);
			default:
				note_num=-1;
				printw("nothing %c\n", note);
				break;
		}
		if(note_num != -1) { 
			generate_tone(playback_handle, 5000, note_num, pitch, 0.1);
		}
	}
	sleep(1);
	snd_pcm_close(playback_handle);
	exit (0);
}
