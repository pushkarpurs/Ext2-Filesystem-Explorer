#ifndef LS_H
#define LS_H

#include "common.h"

void ReadContents(FILE* img, uint32_t blkno);
void List(FILE* img, struct Directory* dirptr);

#endif