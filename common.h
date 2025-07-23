#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct SuperBlock{
		uint32_t blocksize;
		uint16_t inodesize;
		uint32_t firstinode;
		uint32_t totblocks;
		uint32_t totinodes;
		uint32_t inodesgrp;
		uint32_t blocskgrp;
		uint32_t grps;
		uint16_t fstate;
		char* volname;
		bool compression;
		bool typefield;
	};

struct BlockGroup{
		uint32_t blockmap;
		uint32_t inodemap;
		uint32_t inodetable;
		uint16_t unblocks;
		uint16_t uninodes;
		uint16_t dirs;
	};

struct Directory{
		uint32_t inodenumber;
		uint64_t offset;
	};

struct Path{
		struct Path* parent;
		struct Path* child;
		char* name;
};

extern struct SuperBlock* sb;
extern struct BlockGroup* bg;
extern struct Directory* cd;
extern struct Path* bpth;
extern struct Path* epth;

int findtype(FILE* img, uint32_t ino);

#endif