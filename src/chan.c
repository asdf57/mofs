#include "chan.h"

static const char *hello_str = "Hello, World!\n";
static const char *hello_path = "/hello";

int changetattr(const char *path, struct stat *st);
int chanreaddir(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
int chanopen(const char *, struct fuse_file_info *);
int chanread(const char *, char *, size_t, off_t, struct fuse_file_info *);

struct fuse_operations chanhandlers = {
    .getattr = changetattr,
    .readdir = chanreaddir,
    .open = chanopen,
    .read = chanread
};

FSE chanentry = {
    NULL,
    QDIR,
    &chanhandlers,
};

int
changetattr(const char *path, struct stat *st) {
    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);

    logger(INFO, "[changetattr] path: %s\n", path);

    //If we're an actual root call and not a passed up call
    if (strcmp(path, "/channel") == 0) {
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
chanreaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    logger(INFO, "[chanreaddir] path: %s\n", path);

    //Passed up calls
    if (strcmp(path, "/channel") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_path + 1, NULL, 0);

    return 0;
}

int
chanopen(const char *path, struct fuse_file_info *fi) {
    logger(INFO, "[chanopen] path: %s\n", path);

    //Passed up calls
    if (strcmp(path, "/chan") != 0)
        return -ENOENT;

    if ((fi->flags & O_ACCMODE) != O_RDONLY)
        return -EACCES;

    return 0;
}

int
chanread(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    size_t len;
    (void) fi;

    logger(INFO, "[chanread] path: %s\n", path);

    //Passed up calls
    if(strcmp(path, "/chan") != 0)
        return -ENOENT;

    len = strlen(hello_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, hello_str + offset, size);
    } else
        size = 0;

    return size;
}
