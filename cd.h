#ifndef CD_H
#define CD_H

#include "common.h"

bool SelectDir(FILE* img, uint32_t blkno, struct Directory* dir, char* name);
void ChangeDir(FILE* img, struct Directory* dirptr, char* name);

#endif