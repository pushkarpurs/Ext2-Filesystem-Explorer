#include "cat.h"

void ReadFileInode(FILE* img, uint32_t finoffset){
	unsigned char ibuf[sb->inodesize];
	unsigned char bbuf[sb->blocksize];
	fseek(img, finoffset, SEEK_SET);
	if(fread(ibuf,1,sb->inodesize,img)==sb->inodesize){
		uint32_t* fours=(uint32_t*)ibuf;
		fours+=10;
		int i=0;
		for(;i<12;i++){
			if(fours[i]==0){
				return;
			}
			fseek(img,fours[i]*sb->blocksize, SEEK_SET);
			if(fread(bbuf,1,sb->blocksize,img)==sb->blocksize){
				printf("%s",bbuf);
			}
			else{
				printf("Failed to read file block (%d)\n",fours[i]);
				return;
			}
		}
		if(fours[i]==0){
			return;
		}
		unsigned char bbuf1[sb->blocksize];
		uint32_t* fours2;
		fseek(img,fours[i]*sb->blocksize,SEEK_SET);
		if(fread(bbuf1,1,sb->blocksize,img)==sb->blocksize){
			fours2=(uint32_t*)bbuf1;
			for(int sc=0;sc<(sb->blocksize/4);sc++){
				if(fours2[sc]==0){
					return;
				}
				fseek(img,fours2[sc]*sb->blocksize, SEEK_SET);
				if(fread(bbuf,1,sb->blocksize,img)==sb->blocksize){
					printf("%s",bbuf);
				}
				else{
					printf("Failed to read file block (%d)\n",fours2[sc]);
					return;
				}
			}
		}
		else{
			printf("Unable to read singly indirect block\n");
			return;
		}
		i+=1;
		if(fours[i]==0){
			return;
		}
		unsigned char bbuf2[sb->blocksize];
		uint32_t* fours3;
		fseek(img,fours[i]*sb->blocksize,SEEK_SET);
		if(fread(bbuf1,1,sb->blocksize,img)==sb->blocksize){
			fours2=(uint32_t*)bbuf1;
			for(int sc=0;sc<(sb->blocksize/4);sc++){
				if(fours2[sc]==0){
					return;
				}
				fseek(img,fours2[sc]*sb->blocksize,SEEK_SET);
				if(fread(bbuf2,1,sb->blocksize,img)==sb->blocksize){
					fours3=(uint32_t*)bbuf2;
					for(int sc2=0;sc2<(sb->blocksize/4);sc2++){
						if(fours3[sc2]==0){
							return;
						}
						fseek(img,fours3[sc2]*sb->blocksize, SEEK_SET);
						if(fread(bbuf,1,sb->blocksize,img)==sb->blocksize){
							printf("%s",bbuf);
						}
						else{
							printf("Failed to read file block (%d)\n",fours3[sc2]);
							return;
						}
					}
				}
				else{
					printf("Unable to read singly indirect block from doubly indirect block\n");
				}
			}
		}
		else{
			printf("Unable to read doubly indirect block\n");
			return;
		}
		i+=1;
		if(fours[i]==0){
			return;
		}
		unsigned char bbuf3[sb->blocksize];
		uint32_t* fours4;
		fseek(img,fours[i]*sb->blocksize,SEEK_SET);
		if(fread(bbuf1,1,sb->blocksize,img)==sb->blocksize){
			fours2=(uint32_t*)bbuf1;
			for(int sc=0;sc<(sb->blocksize/4);sc++){
				if(fours2[sc]==0){
					return;
				}
				fseek(img,fours2[sc]*sb->blocksize,SEEK_SET);
				if(fread(bbuf2,1,sb->blocksize,img)==sb->blocksize){
					fours3=(uint32_t*)bbuf2;
					for(int sc2=0;sc2<(sb->blocksize/4);sc2++){
						if(fours3[sc2]==0){
							return;
						}
						fseek(img, fours3[sc2]*sb->blocksize,SEEK_SET);
						if(fread(bbuf3,1,sb->blocksize,img)==sb->blocksize){
							fours4=(uint32_t*)bbuf3;
							for(int sc3=0;sc3<(sb->blocksize/4);sc3++){
								if(fours4[sc3]==0){
									return;
								}
								fseek(img,fours4[sc3]*sb->blocksize, SEEK_SET);
								if(fread(bbuf,1,sb->blocksize,img)==sb->blocksize){
									printf("%s",bbuf);
								}
								else{
									printf("Failed to read file block (%d)\n",fours4[sc3]);
									return;
								}
							}
						}
						else{
							printf("Unable to read singly indirect block from triply indirect block\n");
						}
					}
				}
				else{
					printf("Unable to read doubly indirect block from triply indirect block\n");
				}
			}
		}
		else{
			printf("Unable to read triply indirect block\n");
			return;
		}
	}else{
		printf("Failed to read file inode\n");
		return;
	}
	return;
}

bool SelectFile(FILE* img, uint32_t blkno, struct Directory* dir, char* name){
	unsigned char blkbuf[sb->blocksize];
	uint32_t newbno;
	uint32_t newofst;
	fseek(img,blkno*sb->blocksize, SEEK_SET);
	if(fread(blkbuf,1,sb->blocksize,img)==sb->blocksize){
		uint32_t* fours=(uint32_t*)blkbuf;
		uint16_t* twos=(uint16_t*)blkbuf;
		uint8_t* ones=(uint8_t*)blkbuf;
		int i=0;
		while (i<=((sb->blocksize/4)-2)){
			uint16_t entrysize=twos[2];
			if(fours[0]!=0){
				int ftype;
				if(sb->typefield){
					ftype=ones[7];
				}
				else{
					ftype=findtype(img, fours[0]);
				}
				if(ftype==1 && strcmp(name,(ones+8))==0){
					newbno=(fours[0]-1)/sb->inodesgrp;
					newofst=(fours[0]-1)%sb->inodesgrp;
					newofst*=sb->inodesize;
					newofst=(bg+newbno)->inodetable*sb->blocksize+newofst;
					ReadFileInode(img,newofst);
					return true;
				}
			}
			i+=entrysize/4;
			fours+=entrysize/4;
			twos+=entrysize/2;
			ones+=entrysize;
		}
	}
	else{
		printf("Error in reading directory entries\n");
		return true;
	}
	return false;
}

void Concatenate(FILE* img, struct Directory* dirptr, char* name){
	unsigned char ibuf[sb->inodesize];
	fseek(img, dirptr->offset, SEEK_SET);
	if(fread(ibuf,1,sb->inodesize,img)==sb->inodesize){
		uint32_t* fours=(uint32_t*)ibuf;
		fours+=10;
		int i=0;
		for(;i<12;i++){
			if(fours[i]==0){
				return;
			}
			if(SelectFile(img,fours[i],dirptr,name)){
				return;
			}
		}
		if(fours[i]==0){
			return;
		}
		unsigned char bbuf1[sb->blocksize];
		uint32_t* fours2;
		fseek(img,fours[i]*sb->blocksize,SEEK_SET);
		if(fread(bbuf1,1,sb->blocksize,img)==sb->blocksize){
			fours2=(uint32_t*)bbuf1;
			for(int sc=0;sc<(sb->blocksize/4);sc++){
				if(fours2[sc]==0){
					return;
				}
				if(SelectFile(img,fours2[sc],dirptr,name)){
					return;
				}
			}
		}
		else{
			printf("Unable to read singly indirect block\n");
			return;
		}
		i+=1;
		if(fours[i]==0){
			return;
		}
		unsigned char bbuf2[sb->blocksize];
		uint32_t* fours3;
		fseek(img,fours[i]*sb->blocksize,SEEK_SET);
		if(fread(bbuf1,1,sb->blocksize,img)==sb->blocksize){
			fours2=(uint32_t*)bbuf1;
			for(int sc=0;sc<(sb->blocksize/4);sc++){
				if(fours2[sc]==0){
					return;
				}
				fseek(img,fours2[sc]*sb->blocksize,SEEK_SET);
				if(fread(bbuf2,1,sb->blocksize,img)==sb->blocksize){
					fours3=(uint32_t*)bbuf2;
					for(int sc2=0;sc2<(sb->blocksize/4);sc2++){
						if(fours3[sc2]==0){
							return;
						}
						if(SelectFile(img,fours3[sc2],dirptr,name)){
							return;
						}
					}
				}
				else{
					printf("Unable to read singly indirect block from doubly indirect block\n");
				}
			}
		}
		else{
			printf("Unable to read doubly indirect block\n");
			return;
		}
		i+=1;
		if(fours[i]==0){
			return;
		}
		unsigned char bbuf3[sb->blocksize];
		uint32_t* fours4;
		fseek(img,fours[i]*sb->blocksize,SEEK_SET);
		if(fread(bbuf1,1,sb->blocksize,img)==sb->blocksize){
			fours2=(uint32_t*)bbuf1;
			for(int sc=0;sc<(sb->blocksize/4);sc++){
				if(fours2[sc]==0){
					return;
				}
				fseek(img,fours2[sc]*sb->blocksize,SEEK_SET);
				if(fread(bbuf2,1,sb->blocksize,img)==sb->blocksize){
					fours3=(uint32_t*)bbuf2;
					for(int sc2=0;sc2<(sb->blocksize/4);sc2++){
						if(fours3[sc2]==0){
							return;
						}
						fseek(img, fours3[sc2]*sb->blocksize,SEEK_SET);
						if(fread(bbuf3,1,sb->blocksize,img)==sb->blocksize){
							fours4=(uint32_t*)bbuf3;
							for(int sc3=0;sc3<(sb->blocksize/4);sc3++){
								if(fours4[sc3]==0){
									return;
								}
								if(SelectFile(img,fours4[sc3],dirptr,name)){
									return;
								}
							}
						}
						else{
							printf("Unable to read singly indirect block from triply indirect block\n");
						}
					}
				}
				else{
					printf("Unable to read doubly indirect block from triply indirect block\n");
				}
			}
		}
		else{
			printf("Unable to read triply indirect block\n");
			return;
		}
	}else{
		printf("Failed to read dir inode\n");
		return;
	}
	printf("File not found\n");
	return;
}