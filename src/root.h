#ifndef ROOT_H
#define ROOT_H

#define FUSE_USE_VERSION 31
#define FILE_OFFSET_BITS 64

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "hash.h"
#include "file.h"
#include "logger.h"

extern char *rootnames[3];
extern FSE root;
extern FSE *rootdir[];
FSE *rootregentries();

#endif