#include "asset.h"
#include "bezierauto.h"
#include "bezierautos.h"
#include "cache.h"
#include "edl.h"
#include "edlsession.h"
#include "file.h"
#include "mwindow.h"
#include "patch.h"
#include "preferences.h"
#include "mainsession.h"
#include "trackcanvas.h"
#include "tracks.h"
#include "transportque.h"
#include "units.h"
#include "vedit.h"
#include "vedits.h"
#include "vframe.h"
#include "vtrack.h"

VEdit::VEdit(EDL *edl, Edits *edits)
 : Edit(edl, edits)
{
}


VEdit::~VEdit() { }

int VEdit::load_properties_derived(FileXML *xml)
{
	channel = xml->tag.get_property("CHANNEL", (int64_t)0);
	return 0;
}





// ================================================== editing



int VEdit::read_frame(VFrame *video_out, 
			int64_t input_position, 
			int direction,
			CICache *cache,
			int use_nudge)
{
	File *file = cache->check_out(asset);
	int result = 0;
	if(use_nudge) input_position += track->nudge;

	if(file)
	{

		input_position = (direction == PLAY_FORWARD) ? input_position : (input_position - 1);

//printf("VEdit::read_frame 4\n");
		file->set_layer(channel);

//printf("VEdit::read_frame 5\n");
//printf("VEdit::read_frame 3 %lld\n", input_position - startproject + startsource);
		file->set_video_position(input_position - startproject + startsource, edl->session->frame_rate);

//printf("VEdit::read_frame 6 channel: %d w: %d h: %d video_out: %p\n", channel, asset->width, asset->height, video_out);
//asset->dump();
		result = file->read_frame(video_out);

//printf("VEdit::read_frame 7 %d\n", video_out->get_compressed_size());
		cache->check_in(asset);

//printf("VEdit::read_frame 8\n");
	}
	else
		result = 1;
	
//for(int i = 0; i < video_out->get_w() * 3 * 20; i++) video_out->get_rows()[0][i] = 128;
	return result;
}

int VEdit::copy_properties_derived(FileXML *xml, int64_t length_in_selection)
{
	return 0;
}

int VEdit::dump_derived()
{
	printf("	VEdit::dump_derived\n");
	printf("		startproject %ld\n", startproject);
	printf("		length %ld\n", length);
}

int64_t VEdit::get_source_end(int64_t default_)
{
	if(!asset) return default_;   // Infinity

	return (int64_t)((double)asset->video_length / asset->frame_rate * edl->session->frame_rate + 0.5);
}