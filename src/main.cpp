#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <string>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <ncurses.h>
#include <SDL2/SDL.h>
#include <list>


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

float note_values[12]={16.35,17.32,18.35,19.45,20.60,21.83,23.12,24.50,25.96,27.50,29.14,30.87};

class Note {
	private:
	public:
		int note;
		int pitch;
		int volume;
		float decay;
		Note(int n, int p, int v) {
			note=n;
			pitch=p;
			volume=v;
			decay=0.99999;
		};
		int value(unsigned int t) {
			float frequency=note_values[note]*(2<<pitch);
			int state=volume*cos(M_PI*t*frequency/(2*44100));
			//int state=volume*(fmod(t, frequency)/frequency);
			return state;
		};
		int decrease_volume() {
			float f_vol=volume*decay;
			volume=(int) f_vol;
			return volume;	
		};
};

list<Note> notes;
unsigned int play_time=0;

void play_state(snd_pcm_t *handle, SoundInfo sound_info=SoundInfo()) {
	int state=0;
	unsigned char data[4096]={0};
	for(int i=0; i<1024; ++i) {
		for(list<Note>::iterator it=notes.begin(); it!=notes.end(); ++it) {
			state+=it->value(play_time);
			if(it->decrease_volume()==0) {
				notes.erase(it++);
			}
		}
		for(int j=0; j<2; ++j) {
			data[4*i+2*j+0]=state%256;
			data[4*i+2*j+1]=state/256;
		}
		play_time++;
		printf("%d %d %d\n", play_time, data[4*i]+data[4*i+1]*256, state);
	}


	snd_pcm_prepare(handle);
	int err=snd_pcm_writei(handle, data, 1024);
	if(err<0) { 
		err=snd_pcm_recover(handle, err, 0);
		printf("we have errors"); exit(1);
	 }
	return;	
}

void generate_tone(snd_pcm_t *handle, int volume, int note, int pitch, float length, SoundInfo sound_info=SoundInfo(), int type=SIN ) {
	unsigned char* data;
	unsigned int number_of_frames;
	float notes[12]={16.35,17.32,18.35,19.45,20.60,21.83,23.12,24.50,25.96,27.50,29.14,30.87};
	float frequency=notes[note]*(2<<pitch);
	number_of_frames=(int) (length*(*(sound_info.sample_rate))/sound_info.channels);
	data=(unsigned char *)malloc(number_of_frames*2*sound_info.channels);
	for(int i=0; i<number_of_frames; ++i) {
		short s=volume*(cos(M_PI/2+2*M_PI*i*frequency*length/(number_of_frames))+cos(M_PI/2+3*M_PI*i*frequency*length/(number_of_frames)));
		for(int j=0; j<sound_info.channels; ++j) {
			data[2*(i*sound_info.channels+j)+0]=s%256;
			data[2*(i*sound_info.channels+j)+1]=s>>8;
		}
	}
	int err;
	if(snd_pcm_state(handle)==SND_PCM_STATE_RUNNING) {
	}
	err=snd_pcm_writei(handle, data, number_of_frames);
	if(err<0) {
		err=snd_pcm_recover(handle, err, 1);
		snd_pcm_writei(handle, data, 1);
		err=snd_pcm_writei(handle, data, number_of_frames);
//		if(err=snd_pcm_prepare(handle)<0){error_output("cannot prepare audio interface for use",err);}
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
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window * window = SDL_CreateWindow("SDL2 Keyboard/Mouse events", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
        
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
	if(err=snd_pcm_hw_params_set_channels(playback_handle, hw_params, si.channels)<0){error_output("cannot set channel count",err);}
	if(err=snd_pcm_hw_params(playback_handle, hw_params)<0){error_output("cannot set parameters",err);}
	snd_pcm_hw_params_free(hw_params);
	if(err=snd_pcm_prepare(playback_handle)<0){error_output("cannot prepare audio interface for use",err);}
	
	return playback_handle;	
}

int main (int argc, char *argv[]) {
	int err;
	snd_pcm_t *playback_handle;

	printf("#initialising... ");
	if(argc>=2) {
		playback_handle=initialise(argv[1]);
	} else {
		playback_handle=initialise();
	}
	printf("done\n");

	int pitch=4;
	SDL_Event event;
	int volume=5000;
	int time=0;
	while(true) {
		int note_num=-1;
		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {
				case SDLK_EQUALS: volume+=1000; printf("#raising volume\n"); break;
				case SDLK_MINUS: volume-=1000; printf("#lowering volume\n"); break;
				case SDLK_a: note_num=0; printf("#playing C\n"); break;
				case SDLK_w: note_num=1; printf("#playing C#\n"); break;
				case SDLK_s: note_num=2; printf("#playing D\n"); break;
				case SDLK_e: note_num=3; printf("#playing Eb\n"); break;
				case SDLK_d: note_num=4; printf("#playing E\n"); break;
				case SDLK_f: note_num=5; printf("#playing F\n"); break;
				case SDLK_t: note_num=6; printf("#playing F#\n"); break;
				case SDLK_g: note_num=7; printf("#playing G\n"); break;
				case SDLK_y: note_num=8; printf("#playing G#\n"); break;
				case SDLK_h: note_num=9; printf("#playing A\n"); break;
				case SDLK_u: note_num=10; printf("#playing Bb\n"); break;
				case SDLK_j: note_num=11; printf("#playing B\n"); break;
				case SDLK_q: snd_pcm_close(playback_handle); SDL_Quit(); exit(0); break;
			}	
			if(note_num != -1) { 
				notes.push_back(Note(note_num,pitch,volume));
			}
		} else if(event.type==SDL_QUIT) {
			exit(1);	
		}
		play_state(playback_handle);
	}
	sleep(1);
	snd_pcm_close(playback_handle);
	exit (0);
}
