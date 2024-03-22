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

void
addfse(const char *path, FSE *entry)
{
    //Used to dynamically add a file system entry
    unsigned long hash;

    logger(INFO, "[addfse] path: %s\n", path);

    hash = genhash(path);
    logger(INFO, "[addfse] hash: %lu\n", hash);
    fsehashtab[hash] = entry;
}