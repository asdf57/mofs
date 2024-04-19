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

    //Loop until we find the first handler that can fulfill the request (root's handlers are always defined)
    while (entry->handlers == NULL) {
        entry = entry->parent;
    }

    return entry->handlers->getattr(path, st);
}

static int
fsreaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    FSE *entry;

    logger(INFO, "[fsreaddir] path: %s\n", path);

    entry = getfse(path);
    if (entry == NULL) {
        return -ENOENT;
    }

    //Loop until we find the first handler that can fulfill the request
    while (entry->handlers == NULL) {
        entry = entry->parent;
    }

    return entry->handlers->readdir(path, buf, filler, offset, fi);
}

static int
fsread(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    FSE *entry;

    logger(INFO, "[fsread] path: %s\n", path);

    entry = getfse(path);
    if (entry == NULL) {
        return -ENOENT;
    }

    //Loop until we find the first handler that can fulfill the request
    while (entry->handlers == NULL) {
        entry = entry->parent;
    }

    return entry->handlers->read(path, buf, size, offset, fi);
}

static int
fswrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    FSE *entry;

    logger(INFO, "[fswrite] path: %s\n", path);

    entry = getfse(path);
    logger(INFO, "[fswrite] entry: %p\n", entry);
    if (entry == NULL) {
        return -ENOENT;
    }

    //Loop until we find the first handler that can fulfill the request
    while (entry->handlers == NULL) {
        entry = entry->parent;
    }

    return entry->handlers->write(path, buf, size, offset, fi);
}

static int
fsopen(const char *path, struct fuse_file_info *fi) {
    FSE *entry;

    logger(INFO, "[fsopen] path: %s\n", path);

    entry = getfse(path);
    if (entry == NULL) {
        return -ENOENT;
    }

    //Loop until we find the first handler that can fulfill the request
    while (entry->handlers == NULL) {
        entry = entry->parent;
    }

    return entry->handlers->open(path, fi);
}

static int
fstruncate(const char *path, off_t size) {
    FSE *entry;

    logger(INFO, "[fstruncate] path: %s\n", path);

    entry = getfse(path);
    if (entry == NULL) {
        return -ENOENT;
    }

    //Loop until we find the first handler that can fulfill the request
    while (entry->handlers == NULL) {
        entry = entry->parent;
    }

    return entry->handlers->truncate(path, size);
}

static int
fsftruncate(const char *path, off_t size, struct fuse_file_info *fi) {
    FSE *entry;

    logger(INFO, "[fsftruncate] path: %s\n", path);

    entry = getfse(path);
    if (entry == NULL) {
        return -ENOENT;
    }

    //Loop until we find the first handler that can fulfill the request
    while (entry->handlers == NULL) {
        entry = entry->parent;
    }

    return entry->handlers->ftruncate(path, size, fi);
}

static struct fuse_operations operations = {
    .getattr = fsgetattr,
    .readdir = fsreaddir,
    .read = fsread,
    .write = fswrite,
    .open = fsopen,
    .truncate = fstruncate,
    .ftruncate = fsftruncate,
};

void
initfs() {
    initchan();
    initroot();
}

int
main (int argc, char **argv) {
    initfs();
    return fuse_main(argc, argv, &operations, NULL);    
}
