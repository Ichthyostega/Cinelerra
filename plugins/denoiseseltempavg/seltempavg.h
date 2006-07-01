#ifndef SELTEMPAVG_H
#define SELTEMPAVG_H

class SelTempAvgMain;

#include "bchash.inc"
#include "pluginvclient.h"
#include "seltempavgwindow.h"
#include "vframe.inc"

class SelTempAvgConfig
{
public:
	SelTempAvgConfig();
	void copy_from(SelTempAvgConfig *src);
	int equivalent(SelTempAvgConfig *src);

	int frames;

	float avg_threshold_RY, avg_threshold_GU, avg_threshold_BV;
	float std_threshold_RY, std_threshold_GU, std_threshold_BV;
	int mask_RY, mask_GU, mask_BV;

	int method;
	enum
	{
		METHOD_NONE,
		METHOD_SELTEMPAVG,
		METHOD_AVERAGE,
		METHOD_STDDEV
	};

	int offsetmode;
	enum
	{
		OFFSETMODE_FIXED,
		OFFSETMODE_RESTARTMARKERSYS
	};


	int paranoid;
	int nosubtract;
	int offset_restartmarker_keyframe;
	int offset_fixed_value;
	float gain;

};


class SelTempAvgMain : public PluginVClient
{
public:
	SelTempAvgMain(PluginServer *server);
	~SelTempAvgMain();

// required for all realtime plugins
	int process_buffer(VFrame *frame, int64_t start_position, double frame_rate);
	int is_realtime();
	char* plugin_title();
	VFrame* new_picon();
	int show_gui();
	int load_configuration();
	int set_string();
	int load_defaults();
	int save_defaults();
	void save_data(KeyFrame *keyframe);
	void read_data(KeyFrame *keyframe);
	int nextkeyframeisoffsetrestart(KeyFrame *keyframe);
	void raise_window();
	void update_gui();
	void clear_accum(int w, int h, int color_model);
	void subtract_accum(VFrame *frame);
	void add_accum(VFrame *frame);
	void transfer_accum(VFrame *frame);

	int64_t restartoffset;
	int onakeyframe;

	char string[64];		

	VFrame **history;

// Frame of history in requested framerate
	int64_t *history_frame;
	int *history_valid;
	unsigned char *accumulation;
	unsigned char *accumulation_sq;
	unsigned char *accumulation_grey;

// a thread for the GUI
	SelTempAvgThread *thread;
	SelTempAvgConfig config;
	int history_size;
// Starting frame of history in requested framerate
	int64_t history_start;
// When subtraction is disabled, this detects no change for paranoid mode.
	int64_t prev_frame;

	BC_Hash *defaults;
};


#endif
