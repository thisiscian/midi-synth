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
int complain_if_error(int err, string message, bool fatal=false) {
	if(err==0) { return err; }
	cerr << "error: " << message << " (" << err << ")" << endl;
	cerr << "error_message: " << snd_strerror(err) << endl;
	if(fatal==true) { 
		cerr << "fatal: program cannot continue... quitting" << endl;
		exit(1);
	}	
	cerr << endl;
	return err;
}

class MidiDevs {
	public:
		snd_rawmidi_t *input,*output;
		MidiDevs(const char* input_name="virtual", const char* output_name="virtual", int mode=0) {
			int err=complain_if_error(
				snd_rawmidi_open(&input, NULL, input_name, mode),
				"failed to create open input midi device",
				true
			);
			cerr << "error_message: " << snd_strerror(err) << endl;
			cout << "input device: " << snd_rawmidi_name(input) << endl;
			complain_if_error(
				snd_rawmidi_open(NULL, &output, output_name, mode),
				"failed to create open input midi device",
				true
			);
			cout << "output device: " << snd_rawmidi_name(output) << endl;
		}
		int close_input() {
			return complain_if_error(
				snd_rawmidi_close(input),
				"failed to close input midi device"
			);
		}
		int close_output() {
			return complain_if_error(
				snd_rawmidi_close(output),
				"failed to close output midi device"
			);
		}
		int close_all() { return close_input() || close_output(); }
};

int main (int argc, char *argv[]) {
	int err=0;
	MidiDevs midi;
	unsigned char ch[3]={0x90,60,100};
	err=snd_rawmidi_write(midi.output,&ch,3);
	cout << "press enter to continue" << endl;
	cin.ignore();
	err=midi.close_all();
	return err;
}
