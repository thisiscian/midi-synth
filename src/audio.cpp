#include <midi-synth/audio.hpp>
#include <iostream>

Audio::Audio() {
	channels=2;
	sample_rate=44100;
	format=SND_PCM_FORMAT_S16_LE;
	buffer_size=0;
	phase=0;
	ui_phase=0;
	max_phase=2.0*M_PI;

	snd_pcm_hw_params_t *hw_params;
	snd_pcm_hw_params_alloca(&hw_params);

	snd_pcm_sw_params_t *sw_params;
	snd_pcm_sw_params_alloca(&sw_params);
	
	snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	snd_pcm_hw_params_malloc(&hw_params);
	snd_pcm_hw_params_any(playback_handle, hw_params);
	snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(playback_handle, hw_params, format);
	snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &sample_rate, 0);
	snd_pcm_hw_params_set_channels(playback_handle, hw_params, channels);
	snd_pcm_hw_params(playback_handle, hw_params);
	snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_size);
	snd_pcm_hw_params_free(hw_params);

	snd_pcm_sw_params_current(playback_handle, sw_params);
	snd_pcm_sw_params_set_start_threshold(playback_handle, sw_params, buffer_size);
	snd_pcm_sw_params_set_avail_min(playback_handle, sw_params, buffer_size);
	snd_pcm_sw_params(playback_handle, sw_params);

	snd_pcm_prepare(playback_handle);
	outer_samples=(unsigned char*) malloc(buffer_size*channels*snd_pcm_format_physical_width(format)/8);
	areas=(snd_pcm_channel_area_t*) calloc(channels, sizeof(snd_pcm_channel_area_t));
	steps=(int*) calloc(channels, sizeof(int));
	for(int i=0; i<channels; ++i) {
		areas[i].addr=outer_samples;
		areas[i].first=i*snd_pcm_format_physical_width(format);
		areas[i].step=channels*snd_pcm_format_physical_width(format);
		steps[i]=areas[i].step/8;
	}



};

void Audio::add_note(float frequency, int age) {
	for(std::vector< std::pair<float,int> >::iterator it=current_notes.begin(); it!=current_notes.end(); ++it) {
		if(frequency==it->first) { 
			it->second=age;
			return;
		}
	}
	current_notes.push_back(std::pair<float, int>(frequency,age));
}

float Audio::calculate_wave_value(float phase) {
	int size=0;
	float val=0;
	for(std::vector< std::pair<float,int> >::iterator it=current_notes.begin(); it!=current_notes.end(); ++it) {
		if(it->second>0) { val+=sin(it->first*phase); size++; }
	}
	if(size==0) { return 0.0; }
	if(val>1) { val=1; }
	if(val<-1) { val=-1; }
	return val;///size;
}

void Audio::decay() {
	for(std::vector< std::pair<float,int> >::iterator it=current_notes.begin(); it!=current_notes.end(); ++it) {
		if(it->second>0) { it->second--; }
	}
}

void Audio::play() {
	int format_bits=snd_pcm_format_width(format);
	int bps=format_bits/8;
	unsigned char* samples[channels];
	double frame_freq=max_phase*2/(double) sample_rate;
	int usize=buffer_size/10;
	int count=usize;
	unsigned int maxval=(1<<(format_bits-1))-1;
	for(int i=0; i<channels; ++i) {
		samples[i]=(((unsigned char*) areas[i].addr)+(areas[i].first/8));
	}

	while(count-- >0) {
		int fval=calculate_wave_value(frame_freq*ui_phase)*maxval*count/usize;
		for(int i=0; i<channels; ++i) {
			for(int j=0; j<bps; ++j) {
				*(samples[i]+j)=(unsigned char) ((fval >> (j*8))&0xff);
				samples[i]+=steps[i];
			}
		}	
		ui_phase++;
	}	
	decay();
	//ui_phase-=(int) snd_pcm_avail(playback_handle)/2;
	snd_pcm_drain(playback_handle);
	snd_pcm_prepare(playback_handle);	
	int err=snd_pcm_writei(playback_handle, outer_samples,usize);
	ui_phase-=(usize-err);
}

void Audio::play_sine(float freq, float length) {
	int format_bits=snd_pcm_format_width(format);
	int bps=format_bits/8;
	unsigned char* samples[channels];
	double frame_freq=max_phase*2/(double) sample_rate;
	double step=max_phase*freq*2/(double)sample_rate;
	int big_count=length*sample_rate; 
	if(big_count > buffer_size) { big_count=buffer_size; }
	int count=big_count;
	unsigned int maxval=(1<<(format_bits-1))-1;
	for(int i=0; i<channels; ++i) {
		samples[i]=(((unsigned char*) areas[i].addr)+(areas[i].first/8));
	}

	add_note(freq, count);
	while(count-- >0) {
		int fval=calculate_wave_value(frame_freq*ui_phase)*maxval;
		//int fval=sin(frame_freq*freq*ui_phase)*maxval; //calculate_wave_value(phase)*maxval;
		for(int i=0; i<channels; ++i) {
			for(int j=0; j<bps; ++j) {
				*(samples[i]+j)=(unsigned char) ((fval >> (j*8))&0xff);
				samples[i]+=steps[i];
			}
		}	
		phase+=step;
		ui_phase++;
		if(phase>=max_phase) { phase-=max_phase;}
	}	
	decay();
	snd_pcm_drop(playback_handle);
	snd_pcm_prepare(playback_handle);	
	snd_pcm_writei(playback_handle, outer_samples, big_count);
};
