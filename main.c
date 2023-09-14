#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/*
  mofs - A statically allocated FS
*/

#define DIR_ENTRY_MAX 128
#define TABLE_SIZE 128
#define MAX_ENTRIES 1024

enum {
  DIR = 1,
  FIEL = 2,
};

typedef struct Qid {
  unsigned long bucket;
  char path[128];
} Qid;


/*
  Struct holding file metadata
  - Used to check if path is to file or dir
  - 
*/
typedef struct FSEntry {
  int magic;
  Qid qid;
  struct FSEntry *entries[128];
  int num_entries;
  struct FSEntry *next;
} FSEntry;


/*
  /
  ====
  - file create handler - dynamically registers new parent directory
*/
typedef struct handlers {
  void (*file_create_handler)(void);
  void (*read_handler)(void);
} handlers;

FSEntry *entries[TABLE_SIZE];
FSEntry pool[MAX_ENTRIES];
int next_free_node = 0;


unsigned long
get_hash(char *str) {
    unsigned long hash;
    int c;

    hash = 5381;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

unsigned long
get_bucket(char *key) {
  return get_hash(key) % TABLE_SIZE;
}

void
get_parent_entry(const char *path, char *res) {
  if (path == NULL || strlen(path) == 0)
    return;

  if (*path != '/')
    return;

  strcpy(res, path);

  for (res = res + 1; *res != '/' && *res != '\0'; res++);
  *res = '\0';
}

void
add_entry(FSEntry *entries[], FSEntry entry) {
  unsigned int bucket;

  if (next_free_node >= MAX_ENTRIES) {
    fprintf(stderr, "ENOMEM: Out of memory!");
    return;
  }

  bucket = get_bucket(entry.qid.path);

  // First add entry to global static allocation
  pool[next_free_node++] = entry;

  if (entries[bucket] == NULL) {
    entries[bucket] = &pool[next_free_node-1];
  } else {
    pool[next_free_node-1].next = entries[bucket];
    entries[bucket] = &pool[next_free_node-1];
  }
}

FSEntry*
lookup_entry(char *key) {
  FSEntry *entry;
  unsigned long bucket;

  bucket = get_bucket(key);
  entry =  entries[bucket];

  while (entry != NULL && strcmp(key, entry->qid.path) != 0) {
    entry = entry->next;
  }

  return entry;
}

int
set_file_attr(FSEntry *entry, struct stat *st) {
  int res;

  res = 0;

  st->st_uid = getuid();
  st->st_gid = getgid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);
  
  if (entry->magic == DIR) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;
  } else if (entry->magic == FIEL) {
    st->st_mode = S_IFREG | 0755;
    st->st_nlink = 1;
  } else
    res = -ENOENT;

  return res;
}

/*
  Get attribute file hook
*/
static int
do_getattr(const char *path, struct stat *st) {
  int res;
  FSEntry *entry;

  res = 0;

  // Get parent entry for the appropriate handler
  char result[strlen(path)+1];
  get_parent_entry(path, result);
  printf("Path '%s' has parent '%s'\n", path, result);

  // speed up searching to find FSEntry
  // finding FSEntry will give us type.
  if ((entry = lookup_entry(path)) == NULL) {
    // fprintf(stderr, "Couldn't find entry for path %s\n", path);
    return -ENOENT;
  }
  
  printf("entry's stored path: %s\n", entry->qid.path);
  printf("path: %s\n", entry->qid.path);
  printf("bucket: %lu\n", entry->qid.bucket);

  return set_file_attr(entry, st);
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
  FSEntry *entry, *parent;
  char parent_name[256];

  get_parent_entry(path, parent_name);
  entry = lookup_entry(parent_name);
  printf("parent path: %s\n", entry->qid.path);  
}

static int
fs_mknod(const char *path, mode_t mode, dev_t dev) {
  FSEntry *entry;

  if ((entry = lookup_entry(path)) == NULL) {
    printf("Creating entry: %s\n", path);
    add_entry(entries, (FSEntry) {DIR, (Qid) {get_bucket(path), path}, NULL, 0});
  } else {
    return -ENOENT;
  }

  return 0;
}

static struct fuse_operations operations = {
  .getattr = do_getattr,
  .readdir = do_readdir,
  .read    = do_read,
  .mknod   = fs_mknod,
};

void
fs_init(FSEntry *entries[]) {
  add_entry(entries, (FSEntry) {DIR, (Qid) {get_bucket("/"), "/"}, NULL, 0});
  add_entry(entries, (FSEntry) {DIR, (Qid) {get_bucket("/channel"), "/channel"}, NULL, 0});
}

int
main(int argc, char **argv) {
  fs_init(entries);
  return fuse_main(argc, argv, &operations, NULL);
}