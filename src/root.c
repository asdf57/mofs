#include "root.h"
#include "chan.h"

int rootgetattr(const char *path, struct stat *st);
int rootreaddir(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
int rootopen(const char *, struct fuse_file_info *);
int rootread(const char *, char *, size_t, off_t, struct fuse_file_info *);

struct fuse_operations roothandlers = {
    .getattr = rootgetattr,
    .readdir = rootreaddir,
    .open = rootopen,
    .read = rootread
};

int nrootentries = 0;
FSE *root;
FSE *rootdir[1024];

int
rootgetattr(const char *path, struct stat *st) {
    FSE *entry;

    logger(INFO, "[rootgetattr] path: %s\n", path);

    entry = getfse(path);
    if (entry == NULL) {
        logger(INFO, "[rootgetattr] path: %s not found\n", path);
        return -ENOENT;
    }

    *st = entry->properties;

    return 0;
}

int
rootreaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    int i;

    (void) offset;
    (void) fi;

    logger(INFO, "[root_readdir] path: %s\n", path);

    //Passed up calls
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for (i = 0; i < nrootentries; i++) {
        printf("registering rootdir[i]->path: %s\n", rootdir[i]->path);
        filler(buf, rootdir[i]->path + 1, NULL, 0);
    }

    return 0;
}

int
rootopen(const char *path, struct fuse_file_info *fi) {
    logger(INFO, "[root_open] path: %s\n", path);

    return -ENOENT;
}

int
rootread(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    size_t len;
    (void) fi;

    logger(INFO, "[root_read] path: %s\n", path);

    return -ENOENT;
}

void
initroot() {
    regentry((FSE) {"/", "", NULL, QDIR, &roothandlers, genstat(QDIR)});
    rootdir[nrootentries++] = chan;
}
