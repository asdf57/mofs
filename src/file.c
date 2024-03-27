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
regentry(FSE entry) {
    unsigned long hash;
    int i;

    logger(INFO, "Adding entry wity propeties: path: %s, name: %s, parent: %p\n", entry.path, entry.name, entry.parent);

    hash = genhash(entry.path);
    fsehashtab[hash] = malloc(sizeof(FSE));
    *fsehashtab[hash] = entry;

    return fsehashtab[hash];
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
