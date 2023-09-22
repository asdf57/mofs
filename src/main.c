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

#include "utils.h"
#include "file.h"
#include "root.h"
#include "channel.h"

/*
  mofs - A statically allocated FS
*/

FSEntry pool[MAX_ENTRIES];
uint8_t pool_bitmap[MAX_ENTRIES / 8] = {0};
FSEntry *fsentry_table[MAX_ENTRIES];


int
get_free_spot() {
	int i;

	for (i = 0; i < MAX_ENTRIES; i++) {
		if ((pool_bitmap[i >> 3] & 0x80 >> i % 8) == 0) {
			pool_bitmap[i >> 3] |= 0x80 >> i % 8;
			break;
		}
  }

	if (i == MAX_ENTRIES - 1) {
		fprintf(stderr, "no free space in FS!\n");
		return -1;
	}

	return i;
}

FSEntry*
add_entry(FSEntry entry) {
	int idx;

	idx = 0;

	if ((idx = get_free_spot()) == -1)
		return NULL;

	entry.id = idx;

	pool[idx] = entry;

	return &pool[idx];
}

void
remove_entry(FSEntry *entry) {
	memset(&pool[entry->id], 0, sizeof(FSEntry));
}

FSEntry *
lookup_entry(const char *path) {
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
execute_handler(FSEntry *entry, FSop op, int nargs, ...) {
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
		return execute_handler((FSEntry*) entry->parent, op, nargs, args);

	res = handler(entry, args);
	va_end(args);

	return res;
}

int
fs_getattr(const char *path, struct stat *st) {
	FSEntry *entry = lookup_entry(path);
	printf("path selected for gettr: %s\n", entry->name);
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
create_entry(FSEntry *entry, int type, const char *name, handler_set handlers, FSEntry *parent, FSContent content) {
	entry->type = type;
	entry->id = 0;
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
	FSEntry root, channel;
	create_entry(
		&root,
		MOFS_DIR,
		"/",
		(handler_set) { .getattr = root_getattr, .readdir = root_readdir },
		NULL,
		(FSContent) {.dir = {.children = {&channel}, .num_children = 1}});

	create_entry(
		&channel,
		MOFS_DIR,
		"/channel",
		(handler_set) { .getattr = channel_getattr, .readdir = channel_readdir },
		&root,
		(FSContent) {.dir = NULL});

  	return fuse_main(argc, argv, &operations, NULL);
}