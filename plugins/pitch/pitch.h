#ifndef PITCH_H
#define PITCH_H



#include "bchash.inc"
#include "fourier.h"
#include "guicast.h"
#include "mutex.h"
#include "pluginaclient.h"
#include "vframe.inc"


class PitchEffect;

class PitchScale : public BC_FPot
{
public:
	PitchScale(PitchEffect *plugin, int x, int y);
	int handle_event();
	PitchEffect *plugin;
};

class PitchWindow : public BC_Window
{
public:
	PitchWindow(PitchEffect *plugin, int x, int y);
	void create_objects();
	void update();
	int close_event();
	PitchScale *scale;
	PitchEffect *plugin;
};

PLUGIN_THREAD_HEADER(PitchEffect, PitchThread, PitchWindow)


class PitchConfig
{
public:
	PitchConfig();


	int equivalent(PitchConfig &that);
	void copy_from(PitchConfig &that);
	void interpolate(PitchConfig &prev, 
		PitchConfig &next, 
		int64_t prev_frame, 
		int64_t next_frame, 
		int64_t current_frame);


	double scale;
};

class PitchFFT : public CrossfadeFFT
{
public:
	PitchFFT(PitchEffect *plugin);
	~PitchFFT();
	int signal_process();
	int read_samples(int64_t output_sample, 
		int samples, 
		double *buffer);
	PitchEffect *plugin;
	
	double *last_phase;
	double *new_freq;
	double *new_magn;
	double *sum_phase;
	double *anal_freq;
	double *anal_magn;
};

class PitchEffect : public PluginAClient
{
public:
	PitchEffect(PluginServer *server);
	~PitchEffect();

	VFrame* new_picon();
	char* plugin_title();
	int is_realtime();
	void read_data(KeyFrame *keyframe);
	void save_data(KeyFrame *keyframe);
	int process_buffer(int64_t size, 
		double *buffer,
		int64_t start_position,
		int sample_rate);
	int show_gui();
	void raise_window();
	int set_string();




	int load_defaults();
	int save_defaults();
	int load_configuration();
	void reset();
	void update_gui();


	BC_Hash *defaults;
	PitchThread *thread;
	PitchFFT *fft;
	PitchConfig config;
};


#endif
