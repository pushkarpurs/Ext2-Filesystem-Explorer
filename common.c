#include "common.h"

struct SuperBlock* sb = NULL;
struct BlockGroup* bg = NULL;
struct Directory* cd = NULL;
struct Path* bpth = NULL;
struct Path* epth = NULL;

int findtype(FILE* img, uint32_t ino){
	uint32_t newbno;
	uint32_t newofst;
	unsigned char ibuf[sb->inodesize];
	uint8_t* v;
	newbno=(ino-1)/sb->inodesgrp;
	newofst=(ino-1)%sb->inodesgrp;
	newofst*=sb->inodesize;
	newofst=(bg+newbno)->inodetable*sb->blocksize+newofst;
	fseek(img,newofst,SEEK_SET);
	if(fread(ibuf,1,sb->inodesize,img)==sb->inodesize){
		v=(uint8_t*)ibuf;
		int ret=v[1]>>4;
		if(ret==0){
			return ret;
		}
		int ra[]={5,3,2,4,1,7,6};
		return ra[ret/2];
	}
	else{
		printf("Error reading file inode");
		return 0;
	}
}
