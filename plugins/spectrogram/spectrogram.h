#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H






#include "bchash.inc"
#include "../parametric/fourier.h"
#include "guicast.h"
#include "mutex.h"
#include "pluginaclient.h"
#include "vframe.inc"




class Spectrogram;
class SpectrogramFFT;


class SpectrogramLevel : public BC_FPot
{
public:
	SpectrogramLevel(Spectrogram *plugin, int x, int y);
	int handle_event();
	Spectrogram *plugin;
};


class SpectrogramWindow : public BC_Window
{
public:
	SpectrogramWindow(Spectrogram *plugin, int x, int y);
	~SpectrogramWindow();

	void create_objects();
	int close_event();
	void update_gui();

	SpectrogramLevel *level;
	Spectrogram *plugin;
	int done;
	BC_SubWindow *canvas;
};



PLUGIN_THREAD_HEADER(Spectrogram, SpectrogramThread, SpectrogramWindow)



class SpectrogramFFT : public CrossfadeFFT
{
public:
	SpectrogramFFT(Spectrogram *plugin);
	~SpectrogramFFT();
	
	int signal_process();
	int read_samples(int64_t output_sample, 
		int samples, 
		double *buffer);

	Spectrogram *plugin;
};


class SpectrogramConfig
{
public:
	SpectrogramConfig();
	double level;
};


class Spectrogram : public PluginAClient
{
public:
	Spectrogram(PluginServer *server);
	~Spectrogram();
	
	VFrame* new_picon();
	char* plugin_title();
	int is_realtime();
	int process_buffer(int64_t size, 
		double *buffer,
		int64_t start_position,
		int sample_rate);
	int show_gui();
	void raise_window();
	int set_string();
	void load_configuration();
	int load_defaults();
	int save_defaults();
	void read_data(KeyFrame *keyframe);
	void save_data(KeyFrame *keyframe);
	void update_gui();
	void render_gui(void *data, int size);	
	
	void reset();

	int done;

	int need_reconfigure;
	BC_Hash *defaults;
	SpectrogramConfig config;
	SpectrogramThread *thread;
	SpectrogramFFT *fft;
	float *data;
	int total_windows;
};


#endif
