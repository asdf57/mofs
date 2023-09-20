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
#define MAX_ENTRIES 1024
#define MAX_CHILDREN 1024 - 1

enum {
  MOFS_DIR = 1,
  MOFS_FILE = 2,
};

typedef struct {
    int (*getattr)(const char*, struct stat *);
    int (*readlink)(const char *, char *, size_t);
    int (*mknod)(const char *, mode_t, dev_t);
    int (*mkdir)(const char *, mode_t);
    int (*unlink)(const char *);
    int (*rmdir)(const char *);
    int (*symlink)(const char *, const char *);
    int (*rename)(const char *, const char *);
    int (*link)(const char *, const char *);
    int (*chmod)(const char *, mode_t);
    int (*chown)(const char *, uid_t, gid_t);
    int (*truncate)(const char *, off_t);
    int (*open)(const char *, struct fuse_file_info *);
} handler_set;

typedef enum {
	GETATTR 	= 1 << 0,
	READLINK	= 1 << 1,
	MKNOD 		= 1 << 2,
	MKDIR 		= 1 << 3,
	UNLINK		= 1 << 4,
	RMDIR		= 1 << 5,
	SYMLINK		= 1 << 6,
	RENAME		= 1 << 7,
	LINK		= 1 << 8,
	CHMOD		= 1 << 9,
	CHOWN		= 1 << 10,
	TRUNCATE	= 1 << 11,
	OPEN		= 1 << 12,
	READDIR		= 1 << 13,
} FSop;

typedef struct {
	int type;
	int id;
	handler_set handlers;
	struct FSEntry *parent;

	union {
		struct {
			char data[1024]; // fix data size for time being
			size_t size;
		} file;

		struct {
			struct FSEntry *children[MAX_CHILDREN];
		} dir;
	};
} FSEntry;

/*
	INODE approach
	==============
	Current implementation is single-pass (no reusable table entries)
*/
FSEntry pool[MAX_ENTRIES];
uint8_t pool_bitmap[MAX_ENTRIES / 8] = {0};
FSEntry *fsentry_table[MAX_ENTRIES];

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

FSEntry*
get_free_spot() {
	int i, cur_bit;

	for (int i = 0; i < MAX_ENTRIES; i++) {
		if ((pool_bitmap[i >> 3] & 128 >> i % 8) == 0)
			break;
  }

	if (i == MAX_ENTRIES - 1) {
		fprintf(stderr, "no free space in FS!\n");
		return NULL;
	}

	return &pool[i];
	
}

FSEntry *
add_entry(FSEntry entry) {
	int i;
	FSEntry *slot;
	
	if ((slot = get_free_spot()) == NULL)
		return NULL;

	*slot = entry;

	return slot;
}

void remove_entry(FSEntry *entry) {
	memset(&pool[entry->id], 0, sizeof(FSEntry));
}

int execute_handler(FSEntry *entry, FSop op, ...) {

}

int fs_getattr(const char *path, struct stat *st) {
	//FSEntry *entry = lookup_entry(path);
	//execute_handler(entry, GETATTR, path, st);
}

int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	//FSEntry *entry = lookup_entry(path);
	//return execute_handler(entry, READDIR, path, buf, filler, offset, fi);
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
	FSEntry root;
	root = (FSEntry) { .type = MOFS_DIR };


  return fuse_main(argc, argv, &operations, NULL);
}