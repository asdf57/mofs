#ifndef FILE_H
#define FILE_H

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
    FSE *parent;
    FST type;
    struct fuse_operations *handlers;
};

extern FSE *fsehashtab[NENTRIES];

FSE *getfse(const char *);

#endif
