#ifndef MAINUNDO_H
#define MAINUNDO_H


#include "bctimer.inc"
#include "filexml.inc"
#include "mwindow.inc"
#include "undostack.inc"

#include <stdint.h>

class MainUndo
{
public:
	MainUndo(MWindow *mwindow);
	~MainUndo();

	void update_undo(char *description, 
		uint32_t load_flags);

// Create a new undo level only if the arguments are different 
// than the last undo level and the last undo happened within a certain time.
// Replaces the last undo level if the arguments are identical.
// Used for tweeking operations.
	void update_undo(char *description, 
		uint32_t load_flags, 
		void *creator);

// Specify whether to change the changes_made attribute
	void update_undo(char *description, 
		uint32_t load_flags, 
		void *creator, 
		int changes_made);

// Used in undo and redo to reset the creators in all the records.
	void reset_creators();

	int undo();
	int redo();

private:
// Entry point for all update commands
	void update_undo_entry(char *description, 
		uint32_t load_flags, 
		void *creator, 
		int changes_made);
	int load_from_undo(FileXML *file, uint32_t load_flags);    // loads undo from the stringfile to the project

	UndoStack *undo_stack;
	MWindow *mwindow;
	Timer *last_update;
};

#endif
