#include "file.h"

FSE *fsehashtab[NENTRIES] = {0};

FSE*
getfse(const char *path)
{
    FSE *entry;
    unsigned long hash;

    logger(INFO, "[getfse] path: %s\n", path);

    hash = genhash(path);
    logger(INFO, "[getfse] hash: %lu\n", hash);
    logger(INFO, "[getfse] fsehashtab[hash]: %p\n", fsehashtab[hash]);
    if (fsehashtab[hash] == NULL) {
        return NULL;
    }

    return fsehashtab[hash];
}

FSE*
addfse(const char *path, FSE *parent, FST type, struct fuse_operations *handlers) {
    unsigned long hash = genhash(path);
    FSE *entry = malloc(sizeof(FSE));
    entry->type = type;
    entry->parent = parent;
    entry->handlers = handlers;
    fsehashtab[hash] = entry;
    return entry;
}
