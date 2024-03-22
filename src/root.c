#include "root.h"

static const char *hello_str = "Hello, World!\n";
static const char *hello_path = "/hello";

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

FSE rootentry = {
    NULL,
    QDIR,
    &roothandlers,
};

int
rootgetattr(const char *path, struct stat *st) {
    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);

    logger(INFO, "[root_getattr] path: %s\n", path);

    //If we're an actual root call and not a passed up call
    if (strcmp(path, "/") == 0) {
    	st->st_mode = S_IFDIR | 0755;
        st->st_size = 1337;
        st->st_nlink = 2;
    } else {
        //passed up call
        return -ENOENT;
    }

    return 0;
}

int
rootreaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    logger(INFO, "[root_readdir] path: %s\n", path);

    //Passed up calls
    if (strcmp(path, "/") != 0)
        return -ENOENT;

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
