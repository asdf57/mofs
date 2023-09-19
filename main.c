#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/*
  mofs - A statically allocated FS
*/


#define NUM_HANDLERS 13

enum {
  MOFS_DIR = 1,
  MOFS_FILE = 2,
};

enum FSop {
	GETATTR 		= 1 << 0,
    	READLINK 	= 1 << 1,
    	MKNOD 		= 1 << 2,
    	MKDIR 		= 1 << 3,
    	UNLINK		= 1 << 4,
    	RMDIR		= 1 << 5,
    	SYMLINK		= 1 << 6,
    	RENAME		= 1 << 7,
    	LINK			= 1 << 8,
    	CHMOD		= 1 << 9,
    	CHOWN		= 1 << 10,
    	TRUNCATE 	= 1 << 11,
    	OPEN 		= 1 << 12,
};

int
bitflag_to_idx(int val) {
	int i;

	if (val % 2 != 0) {
		fprintf(stderr, "val not a pow of 2!\n");
		return -1;
	}

	for (i = 0; 1 < val; val /= 2)
		i++;

	return i;
}

typedef struct {
  void *handlers[NUM_HANDLERS];
} FSEntry;


int fs_getattr(const char *path, struct stat *st) {
	
}

int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	
}

int fs_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	
}

int fs_mknod(const char *path, mode_t mode, dev_t rdev) {
	
}

static int fs_utimens(const char *path, const struct timespec ts[2]) {
	
}

static struct fuse_operations operations = {
  .getattr = fs_getattr,
  .readdir = fs_readdir,
  .read    = fs_read,
  .mknod   = fs_mknod,
  .utimens = fs_utimens,
};

int
main(int argc, char **argv) {
  return fuse_main(argc, argv, &operations, NULL);
}