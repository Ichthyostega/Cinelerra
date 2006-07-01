#ifndef TRANSITIONPOPUP_H
#define TRANSITIONPOPUP_H

#include "guicast.h"
#include "mwindowgui.inc"
#include "plugin.inc"
#include "transition.inc"

class TransitionPopupOn;
class TransitionPopupShow;
class TransitionPopupAttach;
class TransitionPopupDetach;
class TransitionPopupLength;
class TransitionLengthText;

class TransitionLengthThread : public Thread
{
public:
	TransitionLengthThread(MWindow *mwindow, TransitionPopup *popup);
	~TransitionLengthThread();

	void run();
	
	MWindow *mwindow;
	TransitionPopup *popup;
};


class TransitionLengthDialog : public BC_Window
{
public:
	TransitionLengthDialog(MWindow *mwindow, Transition *transition);
	~TransitionLengthDialog();
	
	void create_objects();
	int close_event();
	
	MWindow *mwindow;
	Transition *transition;
	TransitionLengthText *text;
};

class TransitionLengthText : public BC_TumbleTextBox
{
public:
	TransitionLengthText(MWindow *mwindow, 
		TransitionLengthDialog *gui,
		int x,
		int y);
	int handle_event();
	MWindow *mwindow;
	TransitionLengthDialog *gui;
};


class TransitionPopup : public BC_PopupMenu
{
public:
	TransitionPopup(MWindow *mwindow, MWindowGUI *gui);
	~TransitionPopup();

	void create_objects();
	int update(Transition *transition);

// Acquired through the update command as the plugin currently being operated on
	Transition *transition;

// Set when the user clicks a transition.
	MWindow *mwindow;
	MWindowGUI *gui;

// Needed for loading updates
	TransitionPopupOn *on;
	TransitionPopupShow *show;
	TransitionPopupAttach *attach;
	TransitionPopupDetach *detach;
	TransitionPopupLength *length;
	TransitionLengthThread *length_thread;
};


class TransitionPopupAttach : public BC_MenuItem
{
public:
	TransitionPopupAttach(MWindow *mwindow, TransitionPopup *popup);
	~TransitionPopupAttach();

	int handle_event();
	MWindow *mwindow;
	TransitionPopup *popup;
};

class TransitionPopupDetach : public BC_MenuItem
{
public:
	TransitionPopupDetach(MWindow *mwindow, TransitionPopup *popup);
	~TransitionPopupDetach();

	int handle_event();
	MWindow *mwindow;
	TransitionPopup *popup;
};

class TransitionPopupShow : public BC_MenuItem
{
public:
	TransitionPopupShow(MWindow *mwindow, TransitionPopup *popup);
	~TransitionPopupShow();

	int handle_event();
	MWindow *mwindow;
	TransitionPopup *popup;
};

class TransitionPopupOn : public BC_MenuItem
{
public:
	TransitionPopupOn(MWindow *mwindow, TransitionPopup *popup);
	~TransitionPopupOn();

	int handle_event();
	MWindow *mwindow;
	TransitionPopup *popup;
};

class TransitionPopupLength : public BC_MenuItem
{
public:
	TransitionPopupLength(MWindow *mwindow, TransitionPopup *popup);
	~TransitionPopupLength();

	int handle_event();
	MWindow *mwindow;
	TransitionPopup *popup;
};

#endif
