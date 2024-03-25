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
    // logger(INFO, "[getfse]: parent: %s, path: %s\n", fsehashtab[hash]->parent->path, fsehashtab[hash]->path);
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

void
regentry(FSE *entry) {
    unsigned long hash;
    int i;

    hash = genhash(entry->path);
    fsehashtab[hash] = entry;
}

void
regentries(FSE *entries[], int n) {
    unsigned long hash;
    int i;

    for (i = 0; i < n; i++) {
        hash = genhash(entries[i]->path);
        fsehashtab[hash] = entries[i];
    }
}
