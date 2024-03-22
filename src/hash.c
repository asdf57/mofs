#include "hash.h"

unsigned long
genhash(const char *path)
{
	unsigned char *p;
	unsigned long h;

	h = 2166136261UL;
	for(p = (unsigned char *)path; *p; ++p)
		h = (h ^ *p) * 16777619;
	return h % NHASHES;
}
