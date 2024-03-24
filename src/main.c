#define FUSE_USE_VERSION 31
#define FILE_OFFSET_BITS 64

#include <fuse.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "logger.h"
#include "hash.h"
#include "file.h"
#include "root.h"
#include "chan.h"

static int
fsgetattr(const char *path, struct stat *st) {
    FSE *entry;

    logger(INFO, "[fs_getattr] path: %s\n", path);

    entry = getfse(path);
    if (entry == NULL) {
        logger(INFO, "[fs_getattr] path: %s not found\n", path);
        return -ENOENT;
    }

    printf("entry->type: %p\n", entry->type);

    if (entry->handlers == NULL || entry->handlers->getattr == NULL) {
        return entry->parent->handlers->getattr(path, st);
    }

    return entry->handlers->getattr(path, st);
}

static int
fsreaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    FSE *entry;
 
    logger(INFO, "[fs_readdir] path: %s\n", path);

    entry = getfse(path);
    if (entry->handlers == NULL || entry == NULL) {
        return -ENOENT;
    }

    if (entry->handlers->readdir == NULL) {
        return entry->parent->handlers->readdir(path, buf, filler, offset, fi);
    }

    return entry->handlers->readdir(path, buf, filler, offset, fi);
}

static int
fsread(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    FSE *entry;

    logger(INFO, "[fs_read] path: %s\n", path);

    entry = getfse(path);
    if (entry->handlers == NULL || entry == NULL) {
        return -ENOENT;
    }

    if (entry->handlers->readdir == NULL) {
        return entry->parent->handlers->read(path, buf, size, offset, fi);
    }

    return entry->handlers->read(path, buf, size, offset, fi);
}

static struct fuse_operations operations = {
    .getattr = fsgetattr,
    .readdir = fsreaddir,
    .read = fsread,
};

void
initfs() {
    FSE *rootdir;

    rootdir = rootregentries();
    chanregentries(rootdir);
}

int
main (int argc, char **argv) {
    initfs();
    return fuse_main(argc, argv, &operations, NULL);    
}
