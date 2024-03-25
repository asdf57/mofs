#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <stddef.h>
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
};

extern FSE *fsehashtab[NENTRIES];

FSE *getfse(const char *);
FSE* addfse(const char *, FSE *, FST, struct fuse_operations *);
void regentry(FSE *);
void regentries(FSE *[], int);

#endif
