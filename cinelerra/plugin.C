#include "edl.h"
#include "edlsession.h"
#include "filexml.h"
#include "keyframe.h"
#include "keyframes.h"
#include "localsession.h"
#include "mwindow.h"
#include "messages.h"
#include "plugin.h"
#include "pluginpopup.h"
#include "pluginset.h"
#include "pluginserver.h"
#include "renderengine.h"
#include "track.h"
#include "tracks.h"
#include "virtualnode.h"


Plugin::Plugin(EDL *edl, 
		Track *track, 
		char *title)
 : Edit(edl, track)
{
	this->track = track;
	this->plugin_set = 0;
	strcpy(this->title, title);
	plugin_type = PLUGIN_NONE;
	in = 1;
	out = 1;
	show = 0;
	on = 1;
	keyframes = new KeyFrames(edl, track);
	keyframes->create_objects();
}


Plugin::Plugin(EDL *edl, PluginSet *plugin_set, char *title)
 : Edit(edl, plugin_set)
{
	this->track = plugin_set->track;
	this->plugin_set = plugin_set;
	strcpy(this->title, title);
	plugin_type = PLUGIN_NONE;
	in = 1;
	out = 1;
	show = 0;
	on = 1;
	keyframes = new KeyFrames(edl, track);
	keyframes->create_objects();
}

Plugin::~Plugin()
{
	while(keyframes->last) delete keyframes->last;
	delete keyframes;
}

Edit& Plugin::operator=(Edit& edit)
{
	copy_from(&edit);
	return *this;
}

Plugin& Plugin::operator=(Plugin& edit)
{
	copy_from(&edit);
	return *this;
}

int Plugin::operator==(Plugin& that)
{
	return identical(&that);
}

int Plugin::operator==(Edit& that)
{
	return identical((Plugin*)&that);
}

int Plugin::silence()
{
	if(plugin_type != PLUGIN_NONE) 
		return 0;
	else
		return 1;
}

void Plugin::clear_keyframes(long start, long end)
{
	keyframes->clear(start, end, 0);
}


void Plugin::copy_from(Edit *edit)
{
	Plugin *plugin = (Plugin*)edit;

	this->startsource = edit->startsource;
	this->startproject = edit->startproject;
	this->length = edit->length;


	this->plugin_type = plugin->plugin_type;
	this->in = plugin->in;
	this->out = plugin->out;
	this->show = plugin->show;
	this->on = plugin->on;
// Should reconfigure this based on where the first track is now.
	this->shared_location = plugin->shared_location;
	strcpy(this->title, plugin->title);

	copy_keyframes(plugin);
}

void Plugin::copy_keyframes(Plugin *plugin)
{
	keyframes->copy_from(plugin->keyframes);
}

void Plugin::copy_keyframes(long start, 
	long end, 
	FileXML *file, 
	int default_only,
	int autos_only)
{
	keyframes->copy(start, end, file, default_only, autos_only);
}

void Plugin::synchronize_params(Edit *edit)
{
	Plugin *plugin = (Plugin*)edit;
	this->in = plugin->in;
	this->out = plugin->out;
	this->show = plugin->show;
	this->on = plugin->on;
	strcpy(this->title, plugin->title);
	copy_keyframes(plugin);
}

void Plugin::shift_keyframes(long position)
{
	for(KeyFrame *keyframe = (KeyFrame*)keyframes->first;
		keyframe; 
		keyframe = (KeyFrame*)keyframe->next)
	{
		keyframe->position += position;
	}
}


void Plugin::equivalent_output(Edit *edit, long *result)
{
	Plugin *plugin = (Plugin*)edit;
// End of plugin changed
	if(startproject + length != plugin->startproject + plugin->length)
	{
		if(*result < 0 || startproject + length < *result)
			*result = startproject + length;
	}

// Start of plugin changed
	if(
		startproject != plugin->startproject ||
		plugin_type != plugin->plugin_type ||
		on != plugin->on ||
		!(shared_location == plugin->shared_location) ||
		strcmp(title, plugin->title)
		)
	{
		if(*result < 0 || startproject < *result)
			*result = startproject;
	}

// Test keyframes
	keyframes->equivalent_output(plugin->keyframes, startproject, result);
}



int Plugin::is_synthesis(RenderEngine *renderengine, 
		long position, 
		int direction)
{
	switch(plugin_type)
	{
		case PLUGIN_STANDALONE:
		{
			PluginServer *plugin_server = renderengine->scan_plugindb(title);
			return plugin_server->synthesis;
			break;
		}

// Dereference real plugin and descend another level
		case PLUGIN_SHAREDPLUGIN:
		{
			int real_module_number = shared_location.module;
			int real_plugin_number = shared_location.plugin;
			Track *track = edl->tracks->number(real_module_number);
			Plugin *plugin = track->get_current_plugin(position, 
				real_plugin_number, 
				direction, 
				0);
			return plugin->is_synthesis(renderengine, position, direction);
			break;
		}

// Dereference the real track and descend
		case PLUGIN_SHAREDMODULE:
		{
			int real_module_number = shared_location.module;
			Track *track = edl->tracks->number(real_module_number);
			return track->is_synthesis(renderengine, position, direction);
			break;
		}
	}
	return 0;
}



int Plugin::identical(Plugin *that)
{
	char title1[BCTEXTLEN], title2[BCTEXTLEN];

	calculate_title(title1);
	that->calculate_title(title2);

// printf("Plugin::identical %s %s %d %d %s %s %d\n",
// title1, 
// title2,
// plugin_type, 
// that->plugin_type,
// ((KeyFrame*)keyframes->default_auto)->data,
// ((KeyFrame*)that->keyframes->default_auto)->data,
// ((KeyFrame*)keyframes->default_auto)->identical(((KeyFrame*)that->keyframes->default_auto)));

	return (this->plugin_type == that->plugin_type &&
		this->on == that->on &&
		!strcasecmp(title1, title2) &&
		((KeyFrame*)keyframes->default_auto)->identical(
			((KeyFrame*)that->keyframes->default_auto)));
}

int Plugin::identical_location(Plugin *that)
{
//printf("Plugin::identical_location\n");
	if(!plugin_set || !plugin_set->track) return 0;
	if(!that->plugin_set || !that->plugin_set->track) return 0;

//printf("Plugin::identical_location %d %d %d %d %d %d\n",
//plugin_set->track->number_of(),
//that->plugin_set->track->number_of(),
//plugin_set->get_number(),
//that->plugin_set->get_number(),
//startproject,
//that->startproject);
	if(plugin_set->track->number_of() == that->plugin_set->track->number_of() &&
		plugin_set->get_number() == that->plugin_set->get_number() &&
		startproject == that->startproject) return 1;

	return 0;
}

void Plugin::change_plugin(char *title, 
		SharedLocation *shared_location, 
		int plugin_type)
{
	strcpy(this->title, title);
	this->shared_location = *shared_location;
	this->plugin_type = plugin_type;
}



KeyFrame* Plugin::get_prev_keyframe(long position)
{
	KeyFrame *current = 0;

//printf("Plugin::get_prev_keyframe 1 %p %p\n", this, edl);
// This doesn't work because edl->selectionstart doesn't change during
// playback at the same rate as PluginClient::source_position.
	if(position < 0)
	{
printf("Plugin::get_prev_keyframe position < 0\n");
		position = track->to_units(edl->local_session->selectionstart, 0);
	}
//printf("Plugin::get_prev_keyframe 1 %d\n", position);

	for(current = (KeyFrame*)keyframes->last;
		current;
		current = (KeyFrame*)PREVIOUS)
	{
		if(current->position <= position) break;
	}

//printf("Plugin::get_prev_keyframe %p %p %ld\n", current, keyframes->first, keyframes->first->position);
	if(!current && keyframes->first)
	{
		current = (KeyFrame*)keyframes->first;
	}
	else
	if(!current)
	{
		current = (KeyFrame*)keyframes->default_auto;
	}

//printf("Plugin::get_prev_keyframe 2 %ld %ld\n", 
//	position, current->position);
	return current;
}

KeyFrame* Plugin::get_next_keyframe(long position)
{
	KeyFrame *current;

// This doesn't work because edl->selectionstart doesn't change during
// playback at the same rate as PluginClient::source_position.
	if(position < 0)
	{
printf("Plugin::get_next_keyframe position < 0\n");
		position = track->to_units(edl->local_session->selectionstart, 0);
	}

	for(current = (KeyFrame*)keyframes->first;
		current;
		current = (KeyFrame*)NEXT)
	{
		if(current->position > position) break;
	}

	if(!current && keyframes->last)
	{
		current =  (KeyFrame*)keyframes->last;
	}
	else
	if(!current)
	{
		current = (KeyFrame*)keyframes->default_auto;
	}

//printf("Plugin::get_next_keyframe 2 %ld %ld\n", 
//	position, current->position);
	return current;
}

KeyFrame* Plugin::get_keyframe()
{
// Search for keyframe on or before selection
	KeyFrame *result = 
		get_prev_keyframe(track->to_units(edl->local_session->selectionstart, 0));

//printf("Plugin::get_keyframe %p %p %p\n", result, edl, edl->session);
// Return nearest keyframe if not in automatic keyframe generation
	if(!edl->session->auto_keyframes)
	{
		return result;
	}
	else
// Return new keyframe
	if(result == (KeyFrame*)keyframes->default_auto || 
		result->position != track->to_units(edl->local_session->selectionstart, 0))
	{
		return (KeyFrame*)keyframes->insert_auto(track->to_units(edl->local_session->selectionstart, 0));
	}
	else
// Return existing keyframe
	{
		return result;
	}

	return 0;
}

void Plugin::copy(long start, long end, FileXML *file)
{
	long endproject = startproject + length;

	if((startproject >= start && startproject <= end) ||  // startproject in range
		 (endproject <= end && endproject >= start) ||	   // endproject in range
		 (startproject <= start && endproject >= end))    // range in project
	{
// edit is in range
		long startproject_in_selection = startproject; // start of edit in selection in project
		long startsource_in_selection = startsource; // start of source in selection in source
		long endsource_in_selection = startsource + length; // end of source in selection
		long length_in_selection = length;             // length of edit in selection

		if(startproject < start)
		{         // start is after start of edit in project
			long length_difference = start - startproject;

			startsource_in_selection += length_difference;
			startproject_in_selection += length_difference;
			length_in_selection -= length_difference;
		}

// end is before end of edit in project
		if(endproject > end)
		{         
			length_in_selection = end - startproject_in_selection;
		}

// Plugins don't store silence
		file->tag.set_title("PLUGIN");	
//		file->tag.set_property("STARTPROJECT", startproject_in_selection - start);
		file->tag.set_property("LENGTH", length_in_selection);
		file->tag.set_property("TYPE", plugin_type);
		file->tag.set_property("TITLE", title);
		file->append_tag();
		file->append_newline();


		if(plugin_type == PLUGIN_SHAREDPLUGIN ||
			plugin_type == PLUGIN_SHAREDMODULE)
		{
			shared_location.save(file);
		}



		if(in)
		{
			file->tag.set_title("IN");
			file->append_tag();
		}
		if(out)
		{
			file->tag.set_title("OUT");
			file->append_tag();
		}
		if(show)
		{
			file->tag.set_title("SHOW");
			file->append_tag();
		}
		if(on)
		{
			file->tag.set_title("ON");
			file->append_tag();
		}
		file->append_newline();

// Keyframes
		keyframes->copy(start, end, file, 0, 0);

		file->tag.set_title("/PLUGIN");	
		file->append_tag();
		file->append_newline();
	}
}

void Plugin::load(FileXML *file)
{
	int result = 0;
	int first_keyframe = 1;
 	in = 0;
	out = 0;
// Currently show is ignored when loading
	show = 0;
	on = 0;
	while(keyframes->last) delete keyframes->last;

	do{
		result = file->read_tag();

//printf("Plugin::load 1 %s\n", file->tag.get_title());
		if(!result)
		{
			if(file->tag.title_is("/PLUGIN"))
			{
				result = 1;
			}
			else
			if(file->tag.title_is("SHARED_LOCATION"))
			{
				shared_location.load(file);
			}
			else
			if(file->tag.title_is("IN"))
			{
				in = 1;
			}
			else
			if(file->tag.title_is("OUT"))
			{
				out = 1;
			}
			else
			if(file->tag.title_is("SHOW"))
			{
//				show = 1;
			}
			else
			if(file->tag.title_is("ON"))
			{
				on = 1;
			}
			else
			if(file->tag.title_is("KEYFRAME"))
			{
// Default keyframe
				if(first_keyframe)
				{
					keyframes->default_auto->load(file);
					first_keyframe = 0;
				}
				else
// Override default keyframe
				{
					KeyFrame *keyframe = (KeyFrame*)keyframes->append(new KeyFrame(edl, keyframes));
					keyframe->position = file->tag.get_property("POSITION", (long)0);
					keyframe->load(file);
				}
			}
		}
	}while(!result);
}

void Plugin::get_shared_location(SharedLocation *result)
{
	if(plugin_type == PLUGIN_STANDALONE && plugin_set)
	{
		result->module = edl->tracks->number_of(track);
		result->plugin = track->plugin_set.number_of(plugin_set);
	}
	else
	{
		*result = this->shared_location;
	}
}

Track* Plugin::get_shared_track()
{
	return edl->tracks->get_item_number(shared_location.module);
}

Plugin* Plugin::get_shared_plugin()
{
	Track *track = get_shared_track();

	if(track && 
		shared_location.plugin >= 0)
	{
		return track->get_current_plugin(startproject, 
			shared_location.plugin, 
			PLAY_FORWARD,
			0);
	}
	else
		return 0;
}


void Plugin::calculate_title(char *string)
{
	if(plugin_type == PLUGIN_STANDALONE || plugin_type == PLUGIN_NONE)
	{
		strcpy(string, title);
	}
	else
	if(plugin_type == PLUGIN_SHAREDPLUGIN || plugin_type == PLUGIN_SHAREDMODULE)
	{
		shared_location.calculate_title(string, edl, startproject, 0, plugin_type);
	}
}


void Plugin::paste(FileXML *file)
{
	length = file->tag.get_property("LENGTH", (long)0);
}

void Plugin::resample(double old_rate, double new_rate)
{
// Resample keyframes in here
	keyframes->resample(old_rate, new_rate);
}

void Plugin::shift(long difference)
{
	Edit::shift(difference);
	shift_keyframes(difference);
}

void Plugin::dump()
{
	printf("    PLUGIN: type=%d title=\"%s\" on=%d track=%d plugin=%d\n", 
		plugin_type, 
		title, 
		on, 
		shared_location.module, 
		shared_location.plugin);
	printf("    startproject %ld length %ld\n", startproject, length);

	keyframes->dump();
}

