#include "chan.h"
#include "root.h"

/**
 * Defines the handlers for the /chan directory
*/

static const char *hello_str = "Hello, World!\n";
static const char *hello_path = "/hello";

int changetattr(const char *path, struct stat *st);
int chanreaddir(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
int chanopen(const char *, struct fuse_file_info *);
int chanread(const char *, char *, size_t, off_t, struct fuse_file_info *);
int chanwrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int chantruncate(const char *path, off_t size, struct fuse_file_info *fi);
int chanftruncate(const char *path, off_t size, struct fuse_file_info *fi);

struct fuse_operations chanhandlers = {
    .getattr = changetattr,
    .readdir = chanreaddir,
    .open = chanopen,
    .read = chanread,
    .write = chanwrite,
    .truncate = chantruncate,
    .ftruncate = chanftruncate,
};

FSE chan = {"/chan", "", &root, QDIR, &chanhandlers};
FSE *chandir[] = {
    &(FSE) {"/chan/clone", "clone", &chan, QFILE, NULL},
};

int
changetattr(const char *path, struct stat *st) {
    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);

    logger(INFO, "[changetattr] path: %s\n", path);

    if (strcmp(path, "/chan") == 0) {
    	st->st_mode = S_IFDIR | 0755;
        st->st_size = 1111;
        st->st_nlink = 2;
        return 0;
    } else if (strcmp(path, "/chan/clone") == 0) {
        st->st_mode = S_IFREG | 0444;
        st->st_size = strlen(hello_str);
        st->st_nlink = 1;
        return 0;
    }

    return -ENOENT;
}

int
chanreaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    int i;

    (void) offset;
    (void) fi;

    logger(INFO, "[chanreaddir] path: %s\n", path);

    //Passed up calls
    if (strncmp(path, "/chan", 5) != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for (i = 0; i < 1; i++) {
        filler(buf, chandir[i]->name, NULL, 0);
    }

    return 0;
}

int chanopen(const char *path, struct fuse_file_info *fi) {
    logger(INFO, "[chanopen] path: %s\n", path);

    if (strcmp(path, "/chan/clone") == 0) {
        if ((fi->flags & O_ACCMODE) == O_WRONLY || (fi->flags & O_ACCMODE) == O_RDONLY || (fi->flags & O_ACCMODE) == O_RDWR) {
            logger(INFO, "[chanopen] Opened /chan/clone for writing\n");
            return 0;
        }
    }

    return -ENOENT;
}

int
chanread(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    size_t len;
    (void) fi;

    logger(INFO, "[chanread] path: %s\n", path);

    if (strcmp(path, "/chan/clone") == 0) {
        len = strlen(hello_str);

        if (offset < len) {
            if (offset + size > len)
                size = len - offset;
            memcpy(buf, hello_str + offset, size);
        } else
            size = 0;

        return size;
    }

    return -ENOENT;
}

int chanwrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    (void) offset;

    logger(INFO, "[chanwrite] path: %s, data: %s\n", path, buf);

    if(strcmp(path, "/chan/clone") == 0) {
        printf("%.*s", (int)size, buf);
        return size;
    }

    return -ENOENT;
}

int chantruncate(const char *path, off_t size, struct fuse_file_info *fi) {
    logger(INFO, "[chantruncate] Requested truncate operation on path: %s to size: %lld\n", path, size);
    return 0;
}

int chanftruncate(const char *path, off_t size, struct fuse_file_info *fi) {
    logger(INFO, "[chanftruncate] Requested ftruncate operation on path: %s to size: %lld\n", path, size);
    return chantruncate(path, size, fi);
}
