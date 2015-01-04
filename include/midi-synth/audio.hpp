#ifndef midi_synth_audio
	#define midi_synth_audio
	#include <alsa/asoundlib.h>
	#include <cmath>
	#include <vector>
	#include <utility>
	#define _USE_MATH_DEFINES
	class Audio {
		public:
			unsigned int channels;
			unsigned int sample_rate;
			float phase, max_phase;
			unsigned int ui_phase;
			snd_pcm_format_t format;
			snd_pcm_uframes_t buffer_size;
			snd_pcm_t *playback_handle;
			Audio();
			void add_note(float freq, int length);
			void play_sine(float freq, float length);
			void play();
			void decay();
		private:
			snd_pcm_channel_area_t *areas;
			unsigned char* outer_samples;
			int *steps;
			std::vector< std::pair<float, int> > current_notes;
			float calculate_wave_value(float phase);
	};
#endif 
