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
#include "logger.h"


FSEntry pool[MAX_ENTRIES];
uint8_t pool_bitmap[MAX_ENTRIES / 8] = {0};
FSEntry *fsentry_table[MAX_ENTRIES];

static int
fs_getattr(const char *path, struct stat *st) {
	logger(INFO, "[fs_getattr] path: %s\n", path);
	FSEntry *entry = get_closest_entry(path);
	return execute_handler(entry, GETATTR, 2, path, st);
}

static int
fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	logger(INFO, "[fs_readdir] path: %s\n", path);
	FSEntry *entry = get_closest_entry(path);
	return execute_handler(entry, READDIR, 5, path, buf, filler, offset, fi);
}

static int
fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	logger(INFO, "[fs_read] path: %s\n", path);
	FSEntry *entry = get_closest_entry(path);
	return execute_handler(entry, READ, 5, path, buf, size, offset, fi);
}

static int
fs_mknod(const char *path, mode_t mode, dev_t rdev) {
	logger(INFO, "[fs_mknod] path: %s\n", path);
	FSEntry *entry = get_closest_entry(path);
	return execute_handler(entry, MKNOD, 3, path, mode, rdev);
}

static int
fs_utimens(const char *path, const struct timespec ts[2]) {
	logger(INFO, "[fs_utimens] path: %s\n", path);
	FSEntry *entry = get_closest_entry(path);
	return execute_handler(entry, UTIMENS, 2, path, ts);
}

static int
fs_open(const char *path, struct fuse_file_info *info) {
	logger(INFO, "[fs_open] path: %s\n", path);
	FSEntry *entry = get_closest_entry(path);
	return execute_handler(entry, OPEN, 2, path, info);
}

static int
fs_create(const char *path, mode_t mode, struct fuse_file_info *info) {
	logger(INFO, "[fs_create] path: %s\n", path);
	FSEntry *entry = get_closest_entry(path);
	return execute_handler(entry, CREATE, 3, path, mode, info);
}

static struct fuse_operations operations = {
  .getattr = fs_getattr,
  .readdir = fs_readdir,
  .read    = fs_read,
  .mknod   = fs_mknod,
  .utimens = fs_utimens,
  .open    = fs_open,
  .create  = fs_create,
};

int
main(int argc, char **argv) {
	FSEntry root, channel;

	create_entry(
		&root,
		MOFS_DIR,
		"/",
		(handler_set) {
			.getattr = root_getattr,
			.readdir = root_readdir,
			.create  = root_create,
			.open    = root_open,
		},
		NULL,
		(FSContent) {
			.dir = {
				.children = {&channel},
				.num_children = 1
			}
		});

	create_entry(
		&channel,
		MOFS_DIR,
		"/channel",
		(handler_set) {
			.getattr = channel_getattr,
			.readdir = channel_readdir,
			.read	 = channel_read,
			.mknod	 = channel_mknod,
			.utimens = channel_utimens,
		  .open = channel_open,
		  .create = channel_create,
		},
		&root,
		(FSContent) {.dir = NULL});

  	return fuse_main(argc, argv, &operations, NULL);
}
