#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <stddef.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "logger.h"
#include "hash.h"

typedef struct FSE FSE;
typedef enum FST FST;

enum {
    NENTRIES = 67108859,
};

enum FST {
    QFILE = 0,
    QDIR = 1,
};

struct FSE {
    char path[1024];
    char name[1024];
    FSE *parent;
    FST type;
    struct fuse_operations *handlers;
    struct stat properties;
};

extern FSE *fsehashtab[NENTRIES];

FSE *getfse(const char *);
FSE *regentry(FSE);
void regentries(FSE *[], int);
struct stat genstat(FST type);

#endif
