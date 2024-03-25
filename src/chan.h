#ifndef CHAN_H
#define CHAN_H

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

extern struct fuse_operations chanhandlers;
extern FSE chan;
extern FSE *chandir[];

FSE *chanregentries(FSE*);

#endif