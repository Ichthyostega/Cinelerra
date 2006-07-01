#include "asset.h"
#include "assets.h"
#include "bctimer.h"
#include "edl.h"
#include "filexml.h"
#include "mainindexes.h"
#include "mainmenu.h"
#include "mainsession.h"
#include "mainundo.h"
#include "mwindow.h"
#include "mwindowgui.h"
#include <string.h>
#include "undostack.h"

MainUndo::MainUndo(MWindow *mwindow)
{ 
	this->mwindow = mwindow;
	undo_stack = new UndoStack;
	last_update = new Timer;
}

MainUndo::~MainUndo()
{
	delete undo_stack;
	delete last_update;
}


void MainUndo::update_undo_entry(char *description, 
	uint32_t load_flags,
	void *creator, 
	int changes_made)
{
	FileXML file;

	mwindow->edl->save_xml(mwindow->plugindb, 
		&file, 
		"",
		0,
		0);
	file.terminate_string();
	if(changes_made) mwindow->session->changes_made = 1;

// Test previous entry.
	int need_new = 1;
	UndoStackItem *current_entry;
	if(creator)
	{
		if(undo_stack->current)
		{
			current_entry = undo_stack->current;
			if(last_update->get_difference() < 1000 &&
				current_entry->get_creator() == creator &&
				current_entry->get_description() &&
				!strcmp(current_entry->get_description(), description) &&
				current_entry->get_flags() == load_flags)
				need_new = 0;
		}
	}


	if(need_new)
		current_entry = undo_stack->push();

	current_entry->set_flags(load_flags);
	current_entry->set_data(file.string);
	current_entry->set_description(description);
	current_entry->set_creator(creator);

// Can't undo only 1 record.
	if(undo_stack->total() > 1)
	{
		mwindow->gui->lock_window("MainUndo::update_undo");
		mwindow->gui->mainmenu->undo->update_caption(description);
		mwindow->gui->mainmenu->redo->update_caption("");
		mwindow->gui->unlock_window();
	}
	last_update->update();
}

void MainUndo::update_undo(char *description, uint32_t load_flags)
{
	update_undo_entry(description, load_flags, 0, 1);
}

void MainUndo::update_undo(char *description, 
	uint32_t load_flags,
	void *creator)
{
	update_undo_entry(description, load_flags, creator, 1);
}

void MainUndo::update_undo(char *description, 
	uint32_t load_flags,
	void *creator, 
	int changes_made)
{
	update_undo_entry(description, load_flags, creator, changes_made);
}





int MainUndo::undo()
{
	if(undo_stack->current)
	{
		UndoStackItem *current_entry = undo_stack->current;

		if(current_entry->previous)
		{
			UndoStackItem *prev_entry = current_entry->previous;

// Set the redo text to the current description
			if(current_entry->get_description() && mwindow->gui) 
				mwindow->gui->mainmenu->redo->update_caption(
					current_entry->get_description());

			FileXML file;
			char *prev_data = prev_entry->get_data();
			if(prev_data)
			{
				file.read_from_string(prev_data);
				load_from_undo(&file, current_entry->get_flags());
				delete [] prev_data;

// move current entry back one step
				undo_stack->pull();    


				if(mwindow->gui)
				{
// Must be a previous entry to perform undo
					if(prev_entry->previous)
						mwindow->gui->mainmenu->undo->update_caption(
							prev_entry->get_description());
					else
						mwindow->gui->mainmenu->undo->update_caption("");
				}
			}
		}
	}

	reset_creators();
	return 0;
}

int MainUndo::redo()
{
// Move current entry forward one step
	UndoStackItem *current_entry = undo_stack->pull_next();

	if(current_entry)
	{
		FileXML file;
		char *next_data = current_entry->get_data();
		if(next_data)
		{
			file.read_from_string(next_data);
			load_from_undo(&file, current_entry->get_flags());
			delete [] next_data;

			if(mwindow->gui)
			{
				mwindow->gui->mainmenu->undo->update_caption(current_entry->get_description());
			
				if(current_entry->next)
					mwindow->gui->mainmenu->redo->update_caption(current_entry->next->get_description());
				else
					mwindow->gui->mainmenu->redo->update_caption("");
			}
		}
	}
	reset_creators();
	return 0;
}


// Here the master EDL loads 
int MainUndo::load_from_undo(FileXML *file, uint32_t load_flags)
{
	mwindow->edl->load_xml(mwindow->plugindb, file, load_flags);
	for(Asset *asset = mwindow->edl->assets->first;
		asset;
		asset = asset->next)
	{
		mwindow->mainindexes->add_next_asset(0, asset);
	}
	mwindow->mainindexes->start_build();
	return 0;
}


void MainUndo::reset_creators()
{
	for(UndoStackItem *current = undo_stack->first;
		current;
		current = NEXT)
	{
		current->set_creator(0);
	}
}



