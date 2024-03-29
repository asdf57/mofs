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
int chantruncate(const char *path, off_t size);
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

int nchanentries = 0;
FSE *chan;
FSE *chandir[1024];

int
changetattr(const char *path, struct stat *st) {
    FSE *entry;

    logger(INFO, "[changetattr] path: %s\n", path);

    entry = getfse(path);
    if (entry == NULL) {
        logger(INFO, "[changetattr] path: %s not found\n", path);
        return -ENOENT;
    }

    *st = entry->properties;

    return 0;
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

    for (i = 0; i < nchanentries; i++) {
        filler(buf, chandir[i]->name, NULL, 0);
        logger(INFO, "registering chandir[i]->name: %s\n", chandir[i]->name);
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
    FSE entry;
    long chanid;
    char chanpath[1024];

    (void) fi;
    (void) offset;

    logger(INFO, "[chanwrite] path: %s, data: %s\n", path, buf);

    if(strcmp(path, "/chan/clone") == 0) {
        printf("%.*s", (int)size, buf);

        chanid = strtol(buf, NULL, 10);

        logger(INFO, "Registered ID %d\n", chanid);

        sprintf(chanpath, "/chan/%d", chanid);

        strcpy(entry.path, chanpath);
        sprintf(entry.name, "%ld", chanid);
        entry.parent = chan;
        entry.type = QFILE;
        entry.handlers = NULL;
        entry.properties = genstat(QFILE);
        chandir[nchanentries++] = regentry(entry);

        return size;
    }

    return -ENOENT;
}

int chantruncate(const char *path, off_t size) {
    logger(INFO, "[chantruncate] Requested truncate operation on path: %s to size: %lld\n", path, size);
    return 0;
}

int chanftruncate(const char *path, off_t size, struct fuse_file_info *fi) {
    logger(INFO, "[chanftruncate] Requested ftruncate operation on path: %s to size: %lld\n", path, size);
    return chantruncate(path, size);
}

void
genchanentries() {
    chan = regentry((FSE) {"/chan", "", root, QDIR, &chanhandlers, genstat(QDIR)});
    chandir[nchanentries++] = regentry((FSE) {"/chan/clone", "clone", chan, QFILE, NULL, genstat(QFILE)});
}
