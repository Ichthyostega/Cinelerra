#include "mpeg3private.h"
#include "mpeg3protos.h"

#include <mntent.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

mpeg3_fs_t* mpeg3_new_fs(char *path)
{
	mpeg3_fs_t *fs = calloc(1, sizeof(mpeg3_fs_t));
	fs->buffer = calloc(1, MPEG3_IO_SIZE);
// Force initial read
	fs->buffer_position = -0xffff;
	fs->css = mpeg3_new_css();
	strcpy(fs->path, path);
	return fs;
}

int mpeg3_delete_fs(mpeg3_fs_t *fs)
{
	mpeg3_delete_css(fs->css);
	free(fs->buffer);
	free(fs);
	return 0;
}

int mpeg3_copy_fs(mpeg3_fs_t *dst, mpeg3_fs_t *src)
{
	strcpy(dst->path, src->path);
	dst->current_byte = 0;
	return 0;
}

int64_t mpeg3io_get_total_bytes(mpeg3_fs_t *fs)
{
	struct stat64 ostat;
	stat64(fs->path, &ostat);
	fs->total_bytes = ostat.st_size;
	return fs->total_bytes;
	
/*
 * 	fseek(fs->fd, 0, SEEK_END);
 * 	fs->total_bytes = ftell(fs->fd);
 * 	fseek(fs->fd, 0, SEEK_SET);
 * 	return fs->total_bytes;
 */
}

int64_t mpeg3io_path_total_bytes(char *path)
{
	struct stat64 st;
	if(stat64(path, &st) < 0) return 0;
	return st.st_size;
}

int mpeg3io_open_file(mpeg3_fs_t *fs)
{
/* Need to perform authentication before reading a single byte. */
	mpeg3_get_keys(fs->css, fs->path);

	if(!(fs->fd = fopen64(fs->path, "rb")))
	{
		perror("mpeg3io_open_file");
		return 1;
	}

	fs->total_bytes = mpeg3io_get_total_bytes(fs);

	if(!fs->total_bytes)
	{
		fclose(fs->fd);
		return 1;
	}

	fs->current_byte = 0;
	fs->buffer_position = -0xffff;
	return 0;
}

int mpeg3io_close_file(mpeg3_fs_t *fs)
{
	if(fs->fd) fclose(fs->fd);
	fs->fd = 0;
	return 0;
}

int mpeg3io_read_data(unsigned char *buffer, long bytes, mpeg3_fs_t *fs)
{
	int result = 0, i, fragment_size;
	
//fprintf(stderr, "mpeg3io_read_data 1 %d\n", bytes);
	for(i = 0; bytes > 0 && !result; )
	{
		result = mpeg3io_sync_buffer(fs);

		fragment_size = MPEG3_IO_SIZE;

		if(fragment_size > bytes) fragment_size = bytes;

		if(fs->buffer_offset + fragment_size > fs->buffer_size) 
			fragment_size = fs->buffer_size - fs->buffer_offset;

		memcpy(buffer + i, fs->buffer + fs->buffer_offset, fragment_size);

		fs->buffer_offset += fragment_size;
		fs->current_byte += fragment_size;
		i += fragment_size;
		bytes -= fragment_size;
	}

//fprintf(stderr, "mpeg3io_read_data 2 %d\n", bytes);
	return (result && bytes);
}

int mpeg3io_seek(mpeg3_fs_t *fs, int64_t byte)
{
//printf("mpeg3io_seek 1 %llx\n", byte);
	fs->current_byte = byte;
	return (fs->current_byte < 0) || (fs->current_byte > fs->total_bytes);
}

int mpeg3io_seek_relative(mpeg3_fs_t *fs, long bytes)
{
	fs->current_byte += bytes;
	return (fs->current_byte < 0) || (fs->current_byte > fs->total_bytes);
}

void mpeg3io_read_buffer(mpeg3_fs_t *fs)
{
// Sequential reverse buffer
	if(fs->current_byte == fs->buffer_position - 1)
	{
		fs->buffer_position = fs->current_byte - MPEG3_IO_SIZE + 1;
		if(fs->buffer_position < 0) fs->buffer_position = 0;

		fs->buffer_size = fs->current_byte - fs->buffer_position + 1;
		fs->buffer_offset = fs->buffer_size - 1;

//printf(__FUNCTION__ " 1 %x %x\n", fs->current_byte, fs->buffer_position);
		fseeko64(fs->fd, fs->buffer_position, SEEK_SET);
		fs->buffer_size = fread(fs->buffer, 1, fs->buffer_size, fs->fd);
	}
	else
// Sequential forward buffer or random seek
	{
		int result;
		fs->buffer_position = fs->current_byte;
		fs->buffer_offset = 0;

		result = fseeko64(fs->fd, fs->buffer_position, SEEK_SET);
		fs->buffer_size = fread(fs->buffer, 1, MPEG3_IO_SIZE, fs->fd);
/*
 * printf(__FUNCTION__ " 2 result=%d ftell=%llx buffer_position=%llx %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x\n", 
 * result,
 * ftello64(fs->fd),
 * fs->buffer_position,
 * fs->buffer[0x0],
 * fs->buffer[0x1],
 * fs->buffer[0x2],
 * fs->buffer[0x3],
 * fs->buffer[0x4],
 * fs->buffer[0x5],
 * fs->buffer[0x6],
 * fs->buffer[0x7],
 * fs->buffer[0x12],
 * fs->buffer[0x13]);
 */
	}
}

void mpeg3io_complete_path(char *complete_path, char *path)
{
	if(path[0] != '/')
	{
		char current_dir[MPEG3_STRLEN];
		getcwd(current_dir, MPEG3_STRLEN);
		sprintf(complete_path, "%s/%s", current_dir, path);
	}
	else
		strcpy(complete_path, path);
}

int mpeg3io_device(char *path, char *device)
{
	struct stat64 file_st, device_st;
    struct mntent *mnt;
	FILE *fp;

	if(stat64(path, &file_st) < 0)
	{
		perror("mpeg3io_device");
		return 1;
	}

	fp = setmntent(MOUNTED, "r");
    while(fp && (mnt = getmntent(fp)))
	{
		if(stat64(mnt->mnt_fsname, &device_st) < 0) continue;
		if(device_st.st_rdev == file_st.st_dev)
		{
			strncpy(device, mnt->mnt_fsname, MPEG3_STRLEN);
			break;
		}
	}
	endmntent(fp);

	return 0;
}

void mpeg3io_get_directory(char *directory, char *path)
{
	char *ptr = strrchr(path, '/');
	if(ptr)
	{
		int i;
		for(i = 0; i < ptr - path; i++)
		{
			directory[i] = path[i];
		}
		directory[i] = 0;
	}
}

void mpeg3io_get_filename(char *filename, char *path)
{
	char *ptr = strrchr(path, '/');
	if(!ptr) 
		ptr = path;
	else
		ptr++;

	strcpy(filename, ptr);
}

void mpeg3io_joinpath(char *title_path, char *directory, char *new_filename)
{
	sprintf(title_path, "%s/%s", directory, new_filename);
}


/* Find end of next 4 byte code */
int mpeg3io_next_code(mpeg3_fs_t *fs, uint32_t code, int count)
{
	uint32_t header = 0;

	while(header != code &&
		!mpeg3io_eof(fs) &&
		count > 0)
	{
		header <<= 8;
		header |= mpeg3io_read_char(fs);
		count--;
	}

	return mpeg3io_eof(fs) || count <= 0;
}

/* Find start of previous 4 byte code */
int mpeg3io_prev_code(mpeg3_fs_t *fs, uint32_t code, int count)
{
	uint32_t header = 0;
	while(header != code &&
		!mpeg3io_bof(fs) &&
		count > 0)
	{
		mpeg3io_seek_relative(fs, -1);
		header >>= 8;
		header |= ((uint32_t)mpeg3io_read_char(fs)) << 24;
//printf("mpeg3io_prev_code %08x\n", header);
		mpeg3io_seek_relative(fs, -1);
		count--;
	}
	
	return mpeg3io_bof(fs) || count <= 0;
}
