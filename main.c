#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

const int DIR_ENTRY_MAX = 128;

enum {
  DIR = 1,
  FIEL = 2,
};

typedef struct FSEntry {
  int magic;
  char name[128];
  union {
    struct dir {
      struct FSEntry *subentries[128];
      int entry_count;
    } dir;
    
    char data[128];
  };
} FSEntry;

const FSEntry root[] = {
  {DIR, ".", NULL},
  {DIR, "..", NULL},
  {DIR, "channel",
    {{FIEL, "1", NULL}},
  },
};

int
add_subentry(FSEntry *dir, FSEntry *e) {
  if (dir->dir.entry_count < DIR_ENTRY_MAX)
    dir->dir.subentries[dir->dir.entry_count] = e;

  return 0;
}

/*
  Get attribute file hook
*/
static int do_getattr(const char *path, struct stat *st) {
  st->st_uid = getuid();
  st->st_gid = getgid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);

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

// Reads contents of directory
static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  int i;

  for (i = 0; i < sizeof(root)/sizeof(root[0]); i++) {
    printf("%s\n", root[i].name);
  }
  
  filler(buffer, ".", NULL, 0);
  filler(buffer, "..", NULL, 0);
  filler(buffer, "channel", NULL, 0);
  
  if (strcmp(path, "/") == 0) {
    filler(buffer, "file54", NULL, 0);
    filler(buffer, "file349", NULL, 0);
  }

  if (strcmp(path, "/channel") == 0) {
    filler(buffer, "1", NULL, 0);
  }

  return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
  char selectedText[] = "yahallo";
  if (strcmp(path, "/channel") == 0) {
    
  } else {
    return -1;
  }

  memcpy(buffer, selectedText + offset, size);
  return strlen(selectedText) - offset;
}

static struct fuse_operations operations = {
  .getattr = do_getattr,
  .readdir = do_readdir,
  .read    = do_read,
};

int main(int argc, char **argv) {
  return fuse_main(argc, argv, &operations, NULL);
}