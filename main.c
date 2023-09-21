#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#include "root.h"

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
    int (*getattr)(va_list args);
    int (*readlink)(va_list args);
    int (*mknod)(va_list args);
    int (*mkdir)(va_list args);
    int (*unlink)(va_list args);
    int (*rmdir)(va_list args);
    int (*symlink)(va_list args);
    int (*rename)(va_list args);
    int (*link)(va_list args);
    int (*chmod)(va_list args);
    int (*chown)(va_list args);
    int (*truncate)(va_list args);
    int (*open)(va_list args);
	int (*readdir)(va_list args); 
} handler_set;

typedef int (*generic_handler)(va_list args);

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
	struct FSEntry *children[MAX_CHILDREN];
	size_t num_children;
} dir;

typedef struct {
	char data[1024];
	size_t size;
} file;

typedef union {
	file file;
	dir dir;
} FSContent;

typedef struct {	
	int type;
	int id;
	char name [256];
	void *handlers[14];
	//struct fuse_operations handlers;
	struct FSEntry *parent;
	FSContent content;
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

	for (i = 0; val; val >>= 1, i++)
		if (val & 1)
			return i;

	return -1;
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

FSEntry*
add_entry(FSEntry entry) {
	int i;
	FSEntry *slot;
	
	if ((slot = get_free_spot()) == NULL)
		return NULL;

	entry.id = i;

	*slot = entry;

	return slot;
}

void
remove_entry(FSEntry *entry) {
	memset(&pool[entry->id], 0, sizeof(FSEntry));
}

FSEntry *
lookup_entry(char *path) {
	FSEntry *marker;
	char path_components[10][256];
	char *cur_component;
	int num_components;

	num_components = 0;
	marker = &pool[0];

	return marker; // fix for now

	if (strcmp(path, "/") == 0)
		return marker;

	cur_component = strtok(path, "/");
	while (cur_component != NULL) {
		strcpy(path_components[num_components++], cur_component);
		cur_component = strtok(NULL, "/");
	}
  
  while (marker != NULL || marker->type != MOFS_FILE) {
		//for (int i = 0; )
	}

}

int
execute_handler(FSEntry *entry, FSop op, int nargs, ...) {\
	int res;
	generic_handler handler;
	va_list args;
	va_start(args, nargs);

	if (nargs == 0) {
		va_end(args);
		fprintf(stderr, "no args passed for FUSE handler!\n");
		return -1;
	}

	if (entry->type == MOFS_FILE && op & (MKDIR | RMDIR)) {
		fprintf(stderr, "wrong fs op provided for file!");
		return -1;
	}

	if (op < GETATTR || op > READDIR) {
		fprintf(stderr, "unsupported file system operation!\n");
		return -1;
	}

	handler = (generic_handler) entry->handlers[bitflag_to_idx(op)];

	if (handler == NULL)
		return execute_handler(entry->parent, op, nargs, args);

	res = handler(args);
	va_end(args);

	return res;
}

int
fs_getattr(const char *path, struct stat *st) {
	FSEntry *entry = lookup_entry(path);
	return execute_handler(entry, GETATTR, 2, path, st);
}

int
fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	FSEntry *entry = lookup_entry(path);
	return execute_handler(entry, READDIR, 5, path, buf, filler, offset, fi);
}

int
fs_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	
}

int
fs_mknod(const char *path, mode_t mode, dev_t rdev) {
	
}

static int
fs_utimens(const char *path, const struct timespec ts[2]) {
	
}

void
create_entry(FSEntry *entry, int type, char *name, handler_set handlers, FSEntry *parent, FSContent content) {
	entry->type = type;
	entry->id = NULL;
	strcpy(entry->name, name);

	entry->handlers[bitflag_to_idx(GETATTR)] = handlers.getattr;
	entry->handlers[bitflag_to_idx(READLINK)] = handlers.readlink;
	entry->handlers[bitflag_to_idx(MKNOD)] = handlers.mknod;
	if (type == MOFS_DIR)
		entry->handlers[bitflag_to_idx(MKDIR)] = handlers.mkdir;
	entry->handlers[bitflag_to_idx(UNLINK)] = handlers.unlink;
	if (type == MOFS_DIR)
		entry->handlers[bitflag_to_idx(RMDIR)] = handlers.rmdir;
	entry->handlers[bitflag_to_idx(SYMLINK)] = handlers.symlink;
	entry->handlers[bitflag_to_idx(RENAME)] = handlers.rename;
	entry->handlers[bitflag_to_idx(LINK)] = handlers.link;
	entry->handlers[bitflag_to_idx(CHMOD)] = handlers.chmod;
	entry->handlers[bitflag_to_idx(CHOWN)] = handlers.chown;
	entry->handlers[bitflag_to_idx(TRUNCATE)] = handlers.truncate;
	entry->handlers[bitflag_to_idx(OPEN)] = handlers.open;
	if (type == MOFS_DIR)
		entry->handlers[bitflag_to_idx(READDIR)] = handlers.readdir;

	entry->parent = parent;
	entry->content = content;

	add_entry(*entry);
}

static struct fuse_operations operations = {
  .getattr = fs_getattr,
  .readdir = fs_readdir,
//   .read    = fs_read,
//   .mknod   = fs_mknod,
//   .utimens = fs_utimens,
};

int
main(int argc, char **argv) {
	FSEntry root;
	create_entry(
		&root,
		MOFS_DIR,
		"/",
		(handler_set) { .getattr = root_getattr, .readdir = root_readdir },
		NULL,
		(FSContent) {.dir = NULL});

  	return fuse_main(argc, argv, &operations, NULL);
}