#ifndef midi_synth_display
	#define midi_synth_display
	#include <SDL2/SDL.h>

	class Display {
		public:
			SDL_Window *window;
			SDL_Renderer *renderer;
			int width;
			int height;
			Display(int w=640,int h=480, int flag=0, const char name[]="keyboard simulator");
			void stop();
			void clear();
			void draw();
	};
#endif
