#ifndef _AVC1394Control_H
#define _AVC1394Control_H


#include "bcwindowbase.inc"
#include "librom1394/rom1394.h"
#include "libavc1394/avc1394.h"
#include "libavc1394/avc1394_vcr.h"
#include "mutex.inc"
#include "libraw1394/raw1394.h"
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

class AVC1394Control
{
public:
	AVC1394Control();
	~AVC1394Control();

	void initialize();

	void play();
	void reverse();
	void stop();
	void pause();
	void rewind();
	void fforward();
	void record();
	void eject();
	void get_status();
	void seek(char *time);
	char *timecode();
	int device;
	int status;
	Mutex *device_lock;
// Set by last button pressed to determine the effect of a second press of the
// same button.  Command is from transportque.inc
	int current_command;

private:
	rom1394_directory rom_dir;
	raw1394handle_t handle;
	char text_return[BCTEXTLEN];
};

#endif
