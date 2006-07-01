#ifndef ZOOMPANEL_H
#define ZOOMPANEL_H


#include "guicast.h"
#include "mwindow.inc"

class ZoomPopup;
class ZoomTumbler;

#define ZOOM_PERCENTAGE 0
#define ZOOM_FLOAT 1
#define ZOOM_TIME 2
#define ZOOM_LONG 3

class ZoomHash
{
public:
	ZoomHash(double value, char *text);
	~ZoomHash();

	double value;
	char *text;
} ;

class ZoomPanel
{
public:
	ZoomPanel(MWindow *mwindow, 
		BC_WindowBase *subwindow, 
		double value, 
		int x, 
		int y, 
		int w, 
		double min = 1,
		double max = 131072,
		int zoom_type = ZOOM_PERCENTAGE);
	ZoomPanel(MWindow *mwindow, 
		BC_WindowBase *subwindow, 
		double value, 
		int x, 
		int y, 
		int w, 
		double *user_table,
		int user_size,
		int zoom_type = ZOOM_PERCENTAGE);
	~ZoomPanel();

	virtual int handle_event() { return 1; };
	int get_w();
	void calculate_menu();
	void update_menu();
	int create_objects();
	double get_value();
	char* get_text();
	void set_text(char *text);
	char* value_to_text(double value, int use_table = 1);
	double text_to_zoom(char *text, int use_table = 1);
	void update(double value);
	void update(char *value);
	void reposition_window(int x, int y);
// Set images to be used
	void set_menu_images(VFrame **data);
	void set_tumbler_images(VFrame **data);

	MWindow *mwindow;
	BC_WindowBase *subwindow;
	int x;
	int y;
	int w;
	double value;

	ZoomPopup *zoom_text;
	ZoomTumbler *zoom_tumbler;
	char string[BCTEXTLEN];
	double min, max;
	double *user_table;
	int user_size;
	int zoom_type;
	ArrayList<ZoomHash*> zoom_table;
	VFrame **menu_images;
	VFrame **tumbler_images;
};

class ZoomPopup : public BC_PopupMenu
{
public:
	ZoomPopup(MWindow *mwindow, ZoomPanel *panel, int x, int y);
	~ZoomPopup();

	int handle_event();

	ZoomPanel *panel;
	MWindow *mwindow;
};

class ZoomTumbler : public BC_Tumbler
{
public:
	ZoomTumbler(MWindow *mwindow, ZoomPanel *panel, int x, int y);
	~ZoomTumbler();
	
	int handle_up_event();
	int handle_down_event();
	
	ZoomPanel *panel;
	MWindow *mwindow;
};

#endif 
