#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "file.h"

int
channel_getattr(FSEntry *entry, va_list args) {
	const char *path;
	struct stat *st;

	path = va_arg(args, const char*);
	st = va_arg(args, struct stat *);

	printf("[channel_getattr]\nname: %s\npath:%s\n\n", entry->name, path);

	// file doesn't exist yet
	if (strcmp(entry->path, path) != 0) {
		printf("entry DNE!\n");
		return -ENOENT;
	}

		if (strcmp(path, "/channel") == 0) {
    	st->st_mode = S_IFDIR | 0755;
   		st->st_nlink = 2;
  	} else {
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
 	}


	printf("[channel_getattr] num children: %u\n", entry->content.dir.num_children);
	for (int i = 0; i < entry->content.dir.num_children; i++) {
		printf("Child: %s\n", entry->content.dir.children[i]->name);
	}

	return 0;
}

int
channel_readdir(FSEntry *entry, va_list args) {
	const char *path;
	void *buffer;
	fuse_fill_dir_t filler;
	off_t offset;
	struct fuse_file_info *fi;

	path = va_arg(args, const char *);
	buffer = va_arg(args, void*);
	filler = va_arg(args, fuse_fill_dir_t);
	offset = va_arg(args, off_t);
	fi = va_arg(args, struct fuse_file_info *);

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);

	for (int i = 0; i < entry->content.dir.num_children; i++) {
		printf("Child: %s\n", entry->content.dir.children[i]->name);
		filler(buffer, entry->content.dir.children[i]->path+1, NULL, 0);
	}

	return 0;
}

int
channel_read(FSEntry *entry, va_list args) {
	return 0;
}

int
channel_utimens(FSEntry *entry, va_list args) {
	int res;
	const char *path;
	struct timespec ts[2];
	struct fuse_file_info *fi;

	path = va_arg(args, const char *);
	memcpy(ts, va_arg(args, const struct timespec[2]), sizeof(const struct timespec[2]));

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;


	return 0;
}

int
channel_mknod(FSEntry *entry, va_list args) {
	const char *path;
	mode_t mode;
	dev_t rdev;
	FSEntry new_entry;

	path = va_arg(args, const char *);
	mode = va_arg(args, mode_t);
	rdev = va_arg(args, dev_t);

	printf("\n\n\n[channel_mknod] path: %s\n\n\n", path);

	create_entry(
		&new_entry,
		MOFS_FILE,
		path,
		(handler_set) {},
		entry,
		(FSContent) {.file = NULL});

	return 0;
}

int
channel_open(FSEntry *entry, va_list args) {
	const char *path;
	struct fuse_file_info *info;

	path = va_arg(args, const char *);
	info = va_arg(args, struct fuse_file_info*);

	return 0;
}

int
channel_create(FSEntry *entry, va_list args) {
	const char *path;
	mode_t mode;
	struct fuse_file_info *info;
	FSEntry new_entry;


	path = va_arg(args, const char *);
	mode = va_arg(args, mode_t);
	info = va_arg(args, struct fuse_file_info*);

	printf("in here (channel)\n\n\n\n\n");
		create_entry(
		&new_entry,
		MOFS_FILE,
		path,
		(handler_set) {},
		entry,
		(FSContent) {.file = NULL});

	return 0;
}

