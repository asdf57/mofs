#ifndef FILE_H
#define FILE_H

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>


#define NUM_HANDLERS 13
#define MAX_ENTRIES 1024
#define MAX_CHILDREN 1024 - 1

enum {
  MOFS_DIR = 1,
  MOFS_FILE = 2,
};
typedef struct {
	struct FSEntry *children[MAX_CHILDREN];
	size_t num_children;
} dir;

typedef struct {
	char data[1024];
	size_t size;
} file;

typedef union {
	file file;
	dir dir;
} FSContent;

typedef struct FSEntry {
	int type;
	int id;
	char name [256];
	void *handlers[14];
	struct FSEntry *parent;
	FSContent content;
} FSEntry;

typedef struct {
    int (*getattr)(FSEntry *entry, va_list args);
    int (*readlink)(FSEntry *entry, va_list args);
    int (*mknod)(FSEntry *entry, va_list args);
    int (*mkdir)(FSEntry *entry, va_list args);
    int (*unlink)(FSEntry *entry, va_list args);
    int (*rmdir)(FSEntry *entry, va_list args);
    int (*symlink)(FSEntry *entry, va_list args);
    int (*rename)(FSEntry *entry, va_list args);
    int (*link)(FSEntry *entry, va_list args);
    int (*chmod)(FSEntry *entry, va_list args);
    int (*chown)(FSEntry *entry, va_list args);
    int (*truncate)(FSEntry *entry, va_list args);
    int (*open)(FSEntry *entry, va_list args);
	int (*readdir)(FSEntry *entry, va_list args); 
} handler_set;

typedef int (*generic_handler)(FSEntry *entry, va_list args);

typedef enum {
	GETATTR 	= 1 << 0,
	READLINK	= 1 << 1,
	MKNOD 		= 1 << 2,
	MKDIR 		= 1 << 3,
	UNLINK		= 1 << 4,
	RMDIR		= 1 << 5,
	SYMLINK		= 1 << 6,
	RENAME		= 1 << 7,
	LINK		= 1 << 8,
	CHMOD		= 1 << 9,
	CHOWN		= 1 << 10,
	TRUNCATE	= 1 << 11,
	OPEN		= 1 << 12,
	READDIR		= 1 << 13,
} FSop;

#endif