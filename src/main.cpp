#include <cmath>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <midi-synth.h>
#include <alsa/asoundlib.h>
#include <functional>
#define _USE_MATH_DEFINES
using namespace std;

float standing_wave(float a, float f, float t, float p) { return a*cos(p)*sin(2.0*M_PI*f*t); }
float travelling_wave(float a, float f, float t, float p) { return a*cos(2.0*M_PI*f*t+p); }

int draw_wave(Display display, float frequency, float phase, int r=255, int g=255, int b=255, int a=255) {
	int w=display.width;
	int h=display.height;
	int step=2;
	int err=0;
	int count=w/step+1;
	SDL_Point points[count];
	for(int i=0; i<count; ++i) {
		float pos=i/(float)count;
		points[i].x=i*step;
		points[i].y=0.5*(h+travelling_wave(h,frequency, pos, phase));
	}
	SDL_SetRenderDrawColor(display.renderer, r,g,b,a);
	SDL_RenderDrawLines(display.renderer,points,count);
	return 0;
};

int help() {
	string w="\x1b[30;47m";
	string b="\x1b[37;40m";
	string W=w+" ";	
	string W2=w+"  ";
	string W2a=w+"  \u2502";
	string W3=w+"   \u2502";
	string W3b=w+"   ";
	string B=b+"   ";
	string clear="\x1b[0m";
	cout << "Press ESC to quit" << endl;
	cout << "keys are as follows:\n" << endl;
	cout << "  " << W2+B+W+B+W2a+W2+B+W+B+W+B+W2+clear << "  " << endl;
	cout << "  " << W2+B+W+B+W2a+W2+B+W+B+W+B+W2+clear << "  " << endl;
	cout << "  " << W2+b+" C "+W+b+" D "+W2a+W2+b+" F "+W+b+" G "+W+b+" A "+W2+clear << "  " << endl;
	cout << "  " << W2+b+" # "+W+b+" # "+W2a+W2+b+" # "+W+b+" # "+W+b+" # "+W2+clear << "  " << endl;
	cout << "  " << W3+W3+W3+W3+W3+W3+W3b+clear << "  " << endl;
	cout << "  " << w+" C \u2502 D \u2502 E \u2502 F \u2502 G \u2502 A \u2502 B "+clear << "  " << endl;
	cout << "  " << "                           " << "  " << endl;
	cout << "  " << "    musical keyboard       " << "  " << endl;
	cout << "  " << "           \u21D3          " << "  " << endl;
	cout << "  " << "   computer keyboard       " << "  " << endl;
	cout << "  " << "                           " << "  " << endl;
	cout << "  " << W2+B+W+B+W2a+W2+B+W+B+W+B+W2+clear << "  " << endl;
	cout << "  " << W2+B+W+B+W2a+W2+B+W+B+W+B+W2+clear << "  " << endl;
	cout << "  " << W2+b+" w "+W+b+" e "+W2a+W2+b+" t "+W+b+" y "+W+b+" u "+W2+clear << "  " << endl;
	cout << "  " << W2+b+"   "+W+b+"   "+W2a+W2+b+"   "+W+b+"   "+W+b+"   "+W2+clear << "  " << endl;
	cout << "  " << W3+W3+W3+W3+W3+W3+W3b+clear << "  " << endl;
	cout << "  " << w+" a \u2502 s \u2502 d \u2502 f \u2502 g \u2502 h \u2502 j "+clear << "  " << endl;
	cout << "  " << "\x1b[0m" << "  " << endl;}


int main() {
	Display display;
	Keyboard keyboard;
	keyboard.bind("ESC", [&display]() { display.stop(); exit(0); });

	keyboard.bind("A", [&display,&keyboard]() { float f=16.35; draw_wave(display, f, keyboard.phase, 191,63,21); });
	keyboard.bind("W", [&display,&keyboard]() { float f=17.32; draw_wave(display, f, keyboard.phase, 127,127,42); });
	keyboard.bind("S", [&display,&keyboard]() { float f=18.35; draw_wave(display, f, keyboard.phase, 63,191,63); });
	keyboard.bind("E", [&display,&keyboard]() { float f=19.45; draw_wave(display, f, keyboard.phase, 254,254,84); });
	keyboard.bind("D", [&display,&keyboard]() { float f=20.60; draw_wave(display, f, keyboard.phase, 190,63,106); });
	keyboard.bind("F", [&display,&keyboard]() { float f=21.83; draw_wave(display, f, keyboard.phase, 126,126,127); });
	keyboard.bind("T", [&display,&keyboard]() { float f=23.12; draw_wave(display, f, keyboard.phase, 62,191,148); });
	keyboard.bind("G", [&display,&keyboard]() { float f=24.50; draw_wave(display, f, keyboard.phase, 253,254,169); });
	keyboard.bind("Y", [&display,&keyboard]() { float f=25.96; draw_wave(display, f, keyboard.phase, 189,63,191); });
	keyboard.bind("H", [&display,&keyboard]() { float f=27.50; draw_wave(display, f, keyboard.phase, 125,127,212); });
	keyboard.bind("U", [&display,&keyboard]() { float f=29.14; draw_wave(display, f, keyboard.phase, 61,191,233); });
	keyboard.bind("J", [&display,&keyboard]() { float f=30.87; draw_wave(display, f, keyboard.phase, 252,254,254); });
	help();
	while(1) {
		display.clear();
		keyboard.update();
		display.draw();
	}
	display.stop();
	return 0;	
}
