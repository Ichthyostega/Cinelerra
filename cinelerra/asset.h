#ifndef ASSET_H
#define ASSET_H


#include "arraylist.h"
#include "bcwindowbase.inc"
#include "defaults.inc"
#include "filexml.inc"
#include "linklist.h"
#include "pluginserver.inc"


#include <stdint.h>


// Asset can be one of the following:
// 1) a pure media file
// 2) an EDL
// 3) a log
// The EDL can reference itself if it contains a media file
class Asset : public ListItem<Asset>
{
public:
	Asset();
	Asset(Asset &asset);
	Asset(const char *path);
	Asset(const int plugin_type, const char *plugin_path);
	~Asset();

	int init_values();
	int dump();

	void copy_from(Asset *asset, int do_index);
	void copy_location(Asset *asset);
	void copy_format(Asset *asset, int do_index = 1);
	void copy_index(Asset *asset);
	int64_t get_index_offset(int channel);
	int64_t get_index_size(int channel);
// Get an english description of the compression.  Used by AssetEdit
	char* get_compression_text(int audio, int video);

// Load and save parameters for a render dialog
// Used by render, record, menueffects, preferences
	void load_defaults(Defaults *defaults, 
		char *prefix /* = 0 */, 
		int do_format /* = 0 */,
		int do_compression,
		int do_path,
		int do_data_types,
		int do_bits);
	void save_defaults(Defaults *defaults, 
		char *prefix /* = 0 */,
		int do_format,     /* Don't save format which is autodetected by file loader */
		int do_compression,    /* Don't save compression which is fixed by driver */
		int do_path,
		int do_data_types,
		int do_bits);
	char* construct_param(char *param, char *prefix, char *return_value);




// Executed during index building only
	void update_index(Asset *asset);
	int equivalent(Asset &asset, 
		int test_audio, 
		int test_video);
	Asset& operator=(Asset &asset);
	int operator==(Asset &asset);
	int operator!=(Asset &asset);
	int test_path(const char *path);
	int test_plugin_title(const char *path);
	int read(FileXML *file, int expand_relative = 1);
	int read_audio(FileXML *xml);
	int read_video(FileXML *xml);
	int read_index(FileXML *xml);
	int reset_index();  // When the index file is wrong, reset the asset values
// Output path is the path of the output file if name truncation is desired.
// It is a "" if; complete names should be used.
	int write(FileXML *file, 
		int include_index, 
		char *output_path);
// Write the index data and asset info.  Used by IndexThread.
	int write_index(char *path, int data_bytes);


// Necessary for renderfarm to get encoding parameters
	int write_audio(FileXML *xml);
	int write_video(FileXML *xml);
	int write_index(FileXML *xml);
	int update_path(char *new_path);

// Path to file
	char path[BCTEXTLEN];

// Folder in resource manager
	char folder[BCTEXTLEN];

// Format of file.  An enumeration from file.inc.
	int format;

// contains audio data
	int audio_data;
	int channels;
	int sample_rate;
	int bits;
	int byte_order;
	int signed_;
	int header;
	int dither;
// String or FourCC describing compression
	char acodec[BCTEXTLEN];


	int64_t audio_length;












// contains video data
	int video_data;       
	int layers;
	double frame_rate;
	int width, height;
// String or FourCC describing compression
	char vcodec[BCTEXTLEN];

// Length in units of asset
	int64_t video_length;





// mpeg audio information
	int ampeg_bitrate;
// 2 - 3
	int ampeg_derivative;

// Vorbis compression
	int vorbis_min_bitrate;
	int vorbis_bitrate;
	int vorbis_max_bitrate;
	int vorbis_vbr;

// Theora compression
	int theora_fix_bitrate;
	int theora_bitrate;
	int theora_quality;
	int theora_sharpness;
	int theora_keyframe_frequency;
	int theora_keyframe_force_frequency;


// mp3 compression
	int mp3_bitrate;

// mp4a compression
	int mp4a_bitrate;
	int mp4a_quantqual;


// Set by package render during file creation. -1 means square pixels.
	double aspect_ratio;




// for jpeg compression
	int jpeg_quality;     

// for mpeg video compression
	int vmpeg_iframe_distance;
	int vmpeg_progressive;
	int vmpeg_denoise;
	int vmpeg_seq_codes;
	int vmpeg_bitrate;
// 1 - 2
	int vmpeg_derivative;
	int vmpeg_quantization;
	int vmpeg_cmodel;
	int vmpeg_fix_bitrate;

// mjpegtools
	int vmpeg_preset;
// top field first
	int vmpeg_field_order;
	int vmpeg_pframe_distance;








// Divx video compression
	int divx_bitrate;
	int divx_rc_period;
	int divx_rc_reaction_ratio;
	int divx_rc_reaction_period;
	int divx_max_key_interval;
	int divx_max_quantizer;
	int divx_min_quantizer;
	int divx_quantizer;
	int divx_quality;
	int divx_fix_bitrate;

// h264 video compression
	int h264_bitrate;
	int h264_quantizer;
	int h264_fix_bitrate;

// Divx video decompression
	int divx_use_deblocking;

// PNG video compression
	int png_use_alpha;

// EXR video compression
	int exr_use_alpha;
	int exr_compression;

// TIFF video compression.  An enumeration from filetiff.h
	int tiff_cmodel;
	int tiff_compression;

// Microsoft MPEG-4
	int ms_bitrate;
	int ms_bitrate_tolerance;
	int ms_interlaced;
	int ms_quantization;
	int ms_gop_size;
	int ms_fix_bitrate;


	int ac3_bitrate;



// Image file sequences.  Background rendering doesn't want to write a 
// sequence header but instead wants to start the sequence numbering at a certain
// number.  This ensures deletion of all the frames which aren't being used.
// We still want sequence headers sometimes because loading a directory full of images
// for editing would create new assets for every image.
	int use_header;



// Edits store data for the transition

// index info
	int index_status;     // Macro from assets.inc
	int64_t index_zoom;      // zoom factor of index data
	int64_t index_start;     // byte start of index data in the index file
// Total bytes in source file when the index was buillt
	int64_t index_bytes;
	int64_t index_end, old_index_end;    // values for index build
// offsets of channels in index buffer in floats
	int64_t *index_offsets;
// Sizes of channels in index buffer in floats.  This allows
// variable channel size.
	int64_t *index_sizes;
// [ index channel      ][ index channel      ]
// [high][low][high][low][high][low][high][low]
	float *index_buffer;  
	int id;
};


#endif
