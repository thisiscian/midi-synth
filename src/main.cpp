#include <alsa/asoundlib.h>
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
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

int error_wrapper(int err, const char* msg, bool fatal=false) {
	if(err!=0) {
		cerr << msg << " (" << snd_strerror(err) << ")" << endl;
		if(fatal) { exit(1); }
	}
}
		
float note_frequency(string note_name, float pitch=4.0) {
	float f=0.0;
	if(note_name=="C") 				{ f=16.35; }
	else if(note_name=="C") 	{ f=16.35; }
	else if(note_name=="C#") 	{ f=17.32; }
	else if(note_name=="D") 	{ f=18.35; }
	else if(note_name=="D#") 	{ f=19.45; }
	else if(note_name=="E") 	{ f=20.60; }
	else if(note_name=="F") 	{ f=21.83; }
	else if(note_name=="F#") 	{ f=23.12; }
	else if(note_name=="G") 	{ f=24.50; }
	else if(note_name=="G#") 	{ f=25.96; }
	else if(note_name=="A") 	{ f=27.50; }
	else if(note_name=="A#") 	{ f=29.14; }
	else if(note_name=="B") 	{ f=30.87; }
	else { cerr << "warning, " << note_name << " is a bad note name" << endl; }
	return f*pow(2,pitch);
}

class Sound {
	public:
		string note;
		float pitch;
		int volume;
		float decay;
		float frequency;
		Sound(string n, float p, int v) {
			note=n;
			pitch=p;
			frequency=note_frequency(note,pitch);
			volume=v;
			decay=0.9999999;
		};
		int value(unsigned int t) {
			int state=volume*cos(M_PI*frequency*t/(2*4410));
			//int state=volume*(fmod(t, frequency)/frequency);
			return state;
		};
		int decrease_volume() {
			volume=(int) (volume*decay);
			return volume;	
		};
};

list<Sound> sounds;
unsigned int play_time=0;

void play_state(snd_pcm_t *handle, SoundInfo sound_info=SoundInfo()) {
	int state=0;
	unsigned char data[4096]={0};
	for(int i=0; i<1024; ++i) {
		state=0;
		for(list<Sound>::iterator it=sounds.begin(); it!=sounds.end(); ++it) {
			state+=it->value(play_time);
			if(it->decrease_volume()==0) {
				sounds.erase(it++);
			}
		}
		for(int j=0; j<2; ++j) {
			data[4*i+2*j+0]=state%256;
			data[4*i+2*j+1]=state/256;
		}
		play_time++;
		if (state != 0)	cout << play_time << " " << state << " " << data[4*i] +data[4*i+1]*256 << endl;
	}

	snd_pcm_drain(handle);
	snd_pcm_prepare(handle);
	int err=snd_pcm_writei(handle, data, 1024);
	if(err<0) { 
		err=snd_pcm_recover(handle, err, 0);
		printf("we have errors"); exit(1);
	 }
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
		error_wrapper(err, message, true);
	}

	error_wrapper(snd_pcm_hw_params_malloc(&hw_params),"cannot allocate hardware parameter structure",true);
	error_wrapper(snd_pcm_hw_params_any(playback_handle, hw_params),"cannot initialize hardware parameter structure",true);
	error_wrapper(snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED),"cannot set access type",true);
	error_wrapper(snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE),"cannot set sample format",true);
	error_wrapper(snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, si.sample_rate, 0),"cannot set sample rate",true);
	error_wrapper(snd_pcm_hw_params_set_channels(playback_handle, hw_params, si.channels),"cannot set channel count",true);
	error_wrapper(snd_pcm_hw_params_set_channels(playback_handle, hw_params, si.channels),"cannot set channel count",true);
	error_wrapper(snd_pcm_hw_params(playback_handle, hw_params),"cannot set parameters",true);
	snd_pcm_hw_params_free(hw_params);
	error_wrapper(snd_pcm_prepare(playback_handle),"cannot prepare audio interface for use",true);
	
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
		string note="";
		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {
				case SDLK_EQUALS: volume+=1000; cerr << "raising volume" << endl; break;
				case SDLK_MINUS: volume-=1000; cerr << "lowering volume" << endl; break;
				case SDLK_a: note="C"; break;
				case SDLK_w: note="C#"; break;
				case SDLK_s: note="D"; break;
				case SDLK_e: note="D#"; break;
				case SDLK_d: note="E"; break;
				case SDLK_f: note="F"; break;
				case SDLK_t: note="F#"; break;
				case SDLK_g: note="G"; break;
				case SDLK_y: note="G#"; break;
				case SDLK_h: note="A"; break;
				case SDLK_u: note="A#"; break;
				case SDLK_j: note="B"; break;
				case SDLK_q: snd_pcm_close(playback_handle); SDL_Quit(); exit(0); break;
			}	
			if(note != "") { 
				cout << "#playing " << note << endl;
				sounds.push_back(Sound(note,pitch,volume));
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
