#ifndef CAT_H
#define CAT_H

#include "common.h"

void ReadFileInode(FILE* img, uint32_t finoffset);
bool SelectFile(FILE* img, uint32_t blkno, struct Directory* dir, char* name);
void Concatenate(FILE* img, struct Directory* dirptr, char* name);

#endif