#ifndef TIMEBAR_H
#define TIMEBAR_H

#include "edl.inc"
#include "guicast.h"
#include "filexml.inc"
#include "labels.inc"
#include "mwindow.inc"
#include "recordlabel.inc"
#include "testobject.h"
#include "timebar.inc"
#include "vwindowgui.inc"

class TimeBarLeftArrow;
class TimeBarRightArrow;

class LabelGUI;
class InPointGUI;
class OutPointGUI;
class PresentationGUI;

// Operations for cursor
#define TIMEBAR_NONE        0
#define TIMEBAR_DRAG        1
#define TIMEBAR_DRAG_LEFT   2
#define TIMEBAR_DRAG_RIGHT  3
#define TIMEBAR_DRAG_CENTER 4

class LabelGUI : public BC_Toggle
{
public:
	LabelGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		long pixel, 
		int y, 
		double position,
		VFrame **data = 0);
	virtual ~LabelGUI();

	static int translate_pixel(MWindow *mwindow, int pixel);
	virtual int handle_event();
	static int get_y(MWindow *mwindow, TimeBar *timebar);
	void reposition();

	MWindow *mwindow;
	VWindowGUI *gui;
	TimeBar *timebar;
	long pixel;
	double position;
};

class TestPointGUI : public LabelGUI
{
public:
	TestPointGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		long pixel, 
		double position);
};

class InPointGUI : public LabelGUI
{
public:
	InPointGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		long pixel, 
		double position);
	virtual ~InPointGUI();
	static int get_y(MWindow *mwindow, TimeBar *timebar);
};

class OutPointGUI : public LabelGUI
{
public:
	OutPointGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		long pixel, 
		double position);
	virtual ~OutPointGUI();
	static int get_y(MWindow *mwindow, TimeBar *timebar);
};

class PresentationGUI : public LabelGUI
{
public:
	PresentationGUI(MWindow *mwindow, 
		TimeBar *timebar, 
		long pixel, 
		double position);
	~PresentationGUI();
};

class TimeBar : public BC_SubWindow
{
public:
	TimeBar(MWindow *mwindow, 
		BC_WindowBase *gui,
		int x, 
		int y,
		int w,
		int h);
	virtual ~TimeBar();

	int create_objects();
	int update_defaults();
	int button_press_event();
	int button_release_event();
	int cursor_motion_event();
	int repeat_event(long duration);

// Synchronize label, in/out, presentation display with master EDL
	void update(int do_range = 1, int do_others = 1);
	virtual void draw_time();
// Called by update and draw_time.
	virtual void draw_range();
	virtual void select_label(double position);
	virtual void stop_playback();
	virtual EDL* get_edl();
	virtual int test_preview(int buttonpress);
	virtual void update_preview();
	virtual long position_to_pixel(double position);
	int move_preview(int &redraw);


	void update_labels();
	void update_points();
	void update_presentations();
// Make sure widgets are highlighted according to selection status
	void update_highlights();
// Update highlight cursor during a drag
	void update_cursor();

// ================================= file operations

	int save(FileXML *xml);
	int load(FileXML *xml, int undo_type);

	int delete_project();        // clear timebar of labels

	int draw();                  // draw everything over
	int samplemovement();
	int refresh_labels();

// ========================================= editing

	int copy(long start, long end, FileXML *xml);
	int paste(long start, long end, long sample_length, FileXML *xml);
	int paste_output(long startproject, long endproject, long startsource, long endsource, RecordLabels *new_labels);
	int clear(long start, long end);
	int paste_silence(long start, long end);
	int modify_handles(long oldposition, long newposition, int currentend);
	int select_region(double position);
	void get_edl_length();

	MWindow *mwindow;
	BC_WindowBase *gui;
	int flip_vertical(int w, int h);
	int delete_arrows();    // for flipping vertical

// Operation started by a buttonpress
	int current_operation;


private:
	int get_preview_pixels(int &x1, int &x2);
	int draw_bevel();
	ArrayList<LabelGUI*> labels;
	InPointGUI *in_point;
	OutPointGUI *out_point;
	ArrayList<PresentationGUI*> presentations;


// Records for dragging operations
	double start_position;
	double starting_start_position;
	double starting_end_position;
	double time_per_pixel;
	double edl_length;
	int start_cursor_x;
};


#endif
