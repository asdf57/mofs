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

int root_getattr(FSEntry *entry, va_list args) {
	const char *path;
	struct stat *st;

	path = va_arg(args, const char*);
	st = va_arg(args, struct stat *);

	st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_atime = time(NULL);
	st->st_mtime = time(NULL);

	printf("root_getattr: %s\n", path);

	if (strcmp(path, "/") == 0) {
    	st->st_mode = S_IFDIR | 0755;
   		st->st_nlink = 2;
  	} else {
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;
 	}

	return 0;
}

int root_readdir(FSEntry *entry, va_list args) {
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

	printf("num children: %d\n", entry->content.dir.num_children);

	for (int i = 0; i < entry->content.dir.num_children; i++) {
		printf("[root_readdir] child name: %s\n", entry->content.dir.children[i]->name);
		filler(buffer, entry->content.dir.children[i]->path+1, NULL, 0);
	}

	return 0;
}

int
root_open(FSEntry *entry, va_list args) {
	const char *path;
	struct fuse_file_info *info;

	path = va_arg(args, const char *);
	info = va_arg(args, struct fuse_file_info*);

	return 0;
}

int
root_create(FSEntry *entry, va_list args) {
	const char *path;
	mode_t mode;
	struct fuse_file_info *info;

	path = va_arg(args, const char *);
	mode = va_arg(args, mode_t);
	info = va_arg(args, struct fuse_file_info*);

	printf("in here\n\n\n\n\n");

	return 0;
}

