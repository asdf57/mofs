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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hash.h"
#include "file.h"
#include "logger.h"
#include "proto.h"

extern struct fuse_operations chanhandlers;
extern FSE *chan;
extern FSE *chandir[1024];

void initchan();

#endif