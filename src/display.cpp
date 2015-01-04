#include <midi-synth/display.hpp>

Display::Display(int w,int h, const char name[]) {
	width=w;
	height=h;
	SDL_Init(SDL_INIT_VIDEO);
	window=SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	renderer=SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
};

void Display::stop() {
	SDL_DestroyWindow(window);
	SDL_Quit();
};

void Display::clear() {
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0,0,0,125);
	SDL_Rect rect={0,0,width,height};
	SDL_RenderFillRect(renderer,&rect);	
}

void Display::draw() {
	SDL_RenderPresent(renderer);
};
