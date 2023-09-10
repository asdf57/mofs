#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

const int DIR_ENTRY_MAX = 128;

enum {
  DIR = 1,
  FIEL = 2,
};

/*
int
add_subentry(FSEntry *dir, FSEntry *e) {
  if (dir->dir.entry_count < DIR_ENTRY_MAX)
    dir->dir.children[dir->dir.entry_count] = e;

  return 0;
}
*/

/*
  Get attribute file hook
*/
static int
do_getattr(const char *path, struct stat *st) {
  int res;

  res = 0;

  st->st_uid = getuid();
  st->st_gid = getgid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);

  printf("Provided path: %s\n", path);

  if (strcmp(path, "/") == 0) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;
  } else if (strcmp(path, "/channel") == 0) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;
  } else if (strcmp(path, "/channel/abc") == 0) {
    st->st_mode = S_IFREG | 0755;
    st->st_nlink = 2;
  }
  else
    res = -ENOENT;

  return res;
}

static int
do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {  
  if (strcmp(path, "/") != 0)
    return -ENOENT;

  filler(buffer, ".", NULL, 0);
  filler(buffer, "..", NULL, 0);
  filler(buffer, "channel", NULL, 0);

  return 0;
}

static int
do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {

}

static int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
  if (strcmp(path, "/channel") == 0) {
    printf("Writing file\n");
  }

  return 0;
}


static struct fuse_operations operations = {
  .getattr = do_getattr,
  .readdir = do_readdir,
  .read    = do_read,
  .create  = fs_create,
};

int main(int argc, char **argv) {
  return fuse_main(argc, argv, &operations, NULL);
}