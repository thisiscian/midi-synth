#ifndef midi_synth_keyboard
	#define midi_synth_keyboard
	#include <string>
	#include <SDL2/SDL.h>
	#include <sys/time.h>
	#include <vector>
	#include <map>
	#include <functional>

	class Key {
		public:
			int scancode;
			Key();
			Key(int s);
			void bind(std::function< void(void)> &func);
			void press();
		private:
			std::vector< std::function<void(void)> > bindings;
	};

	class Keyboard {
		public:
			float phase;
			Keyboard();
			int update();
			int bind(std::string key, std::function<void(void)> func);
		private:
			timeval tv;
			std::map<std::string,Key> keys;
			const Uint8 *keyboard_state;
	};
#endif
