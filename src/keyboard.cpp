#include <midi-synth/keyboard.hpp>

Key::Key() {};
Key::Key(int s) {
	scancode=s;
};

void Key::bind(std::function< void(void)> &func) {
	bindings.push_back(func);
};

void Key::press() {
	for(std::vector <std::function<void(void)> >::iterator it=bindings.begin(); it!=bindings.end(); ++it) {
		(*it)();	
	}	
};

Keyboard::Keyboard() {
	keys["ESC"]=Key(SDL_SCANCODE_ESCAPE);
	keys["A"]=Key(SDL_SCANCODE_A);
	keys["W"]=Key(SDL_SCANCODE_W);
	keys["S"]=Key(SDL_SCANCODE_S);
	keys["E"]=Key(SDL_SCANCODE_E);
	keys["D"]=Key(SDL_SCANCODE_D);
	keys["F"]=Key(SDL_SCANCODE_F);
	keys["T"]=Key(SDL_SCANCODE_T);
	keys["G"]=Key(SDL_SCANCODE_G);
	keys["Y"]=Key(SDL_SCANCODE_Y);
	keys["H"]=Key(SDL_SCANCODE_H);
	keys["U"]=Key(SDL_SCANCODE_U);
	keys["J"]=Key(SDL_SCANCODE_J);
};

int Keyboard::update() {
	gettimeofday (&tv, NULL);
	phase=2.0*M_PI*(0.000001 * tv.tv_usec);
	SDL_PumpEvents();
	keyboard_state=SDL_GetKeyboardState(NULL);
	for(std::map<std::string,Key>::iterator it=keys.begin(); it!=keys.end(); ++it) {
		Key k=it->second;
		if(keyboard_state[k.scancode]) k.press();
	}
	return 0;
};

int Keyboard::bind(std::string key, std::function<void(void)> func) {
	keys[key].bind(func);
};
