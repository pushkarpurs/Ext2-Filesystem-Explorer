#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
	}* sb;

struct BlockGroup{
		uint32_t blockmap;
		uint32_t inodemap;
		uint32_t inodetable;
		uint16_t unblocks;
		uint16_t uninodes;
		uint16_t dirs;
	}* bg;

struct Directory{
		uint32_t inodenumber;
		uint64_t offset;
	}* cd;

struct Path{
		struct Path* parent;
		struct Path* child;
		char* name;
}* bpth,* epth;

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

bool SelectDir(FILE* img, uint32_t blkno, struct Directory* dir, char* name){
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
				int etype;
				if(sb->typefield){
					etype=ones[7];
				}
				else{
					etype=findtype(img,fours[0]);
				}
				if(etype==2 && strcmp(name,(ones+8))==0){
					if(strcmp(name,"..")==0){
						struct Path* temp=epth;
						epth=epth->parent;
						free(temp);
						epth->child=NULL;
					}
					else{
						epth->child=calloc(1,sizeof(struct Path));
						epth->child->parent=epth;
						epth=epth->child;
						epth->name=malloc(strlen(ones+8)+1);
						strcpy(epth->name, (ones+8));
					}
					dir->inodenumber=fours[0];
					newbno=(fours[0]-1)/sb->inodesgrp;
					newofst=(fours[0]-1)%sb->inodesgrp;
					newofst*=sb->inodesize;
					dir->offset=(bg+newbno)->inodetable*sb->blocksize+newofst;
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

void ChangeDir(FILE* img, struct Directory* dirptr, char* name){
	if(strcmp(name,".")==0 || (strcmp(name,"..")==0 && epth->parent==NULL)){
		return;
	}
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
			if(SelectDir(img,fours[i],dirptr,name)){
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
				if(SelectDir(img,fours2[sc],dirptr,name)){
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
						if(SelectDir(img,fours3[sc2],dirptr,name)){
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
								if(SelectDir(img,fours4[sc3],dirptr,name)){
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
	printf("Directory not found\n");
	return;
}

void ReadContents(FILE* img, uint32_t blkno){
	const char* file_types[] = {
        "Unknown type",
        "Regular file",
        "Directory",
        "Character device",
        "Block device",
        "FIFO",
        "Socket",
        "Symbolic link (soft link)" 
    };
	unsigned char blkbuf[sb->blocksize];
	fseek(img,blkno*sb->blocksize, SEEK_SET);
	if(fread(blkbuf,1,sb->blocksize,img)==sb->blocksize){
		uint32_t* fours=(uint32_t*)blkbuf;
		uint16_t* twos=(uint16_t*)blkbuf;
		uint8_t* ones=(uint8_t*)blkbuf;
		int i=0;
		while (i<=((sb->blocksize/4)-2)){
			uint16_t entrysize=twos[2];
			if(fours[0]!=0){
				if(sb->typefield){
					printf("%s (%s)",(ones+8), file_types[ones[7]]);
				}
				else{
					printf("%s (%s)",(ones+8), file_types[findtype(img, fours[0])]);
				}
				printf("\n");
			}
			i+=entrysize/4;
			fours+=entrysize/4;
			twos+=entrysize/2;
			ones+=entrysize;
		}
	}
	else{
		printf("Error in reading directory entries");
	}
}

void List(FILE* img, struct Directory* dirptr){
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
			ReadContents(img,fours[i]);
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
				ReadContents(img,fours2[sc]);
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
						ReadContents(img,fours3[sc2]);
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
								ReadContents(img,fours4[sc3]);
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
}

int main(int argc, char *argv[]) {
	#ifdef _WIN32
    	system("cls");
	#else
    	printf("\033[2J\033[H");
	#endif
    if (argc != 2) {
        fprintf(stderr, "Provide image as argument");
        return 1;
    }
	sb=calloc(1,sizeof(struct SuperBlock));
    FILE *img = fopen(argv[1], "rb");
    if (!img) {
        perror("Failed to open image file");
        return 1;
    }

    unsigned char buffer[1024];
    int block_num = 1;
	
	if (fseek(img, block_num * 1024, SEEK_SET) != 0) {
        perror("Failed to seek to block");
		fclose(img);
        return 1;
    }

    if (fread(buffer, 1, 1024, img) == 1024) {
		printf("------------------SuprBlock------------------\n");
		if(buffer[56]==83 && buffer[57]==239)
			printf("The image is in Ext2 format\n");
		else{
			fprintf(stderr, "Image is nhot in Ext2 format");
			fclose(img);
			return 1;
		}
		uint32_t* fours=(uint32_t* )buffer;
		uint16_t* twos=(uint16_t* )buffer; 
		sb->fstate=twos[29];
		if(twos[29]==1)
		{
			printf("File system state is clean\n");
		}
		else{
			printf("File system state isnt clean\n");
		}
		if(ceil(fours[1]*1.0/fours[8])==ceil(fours[0]*1.0/fours[10])){
			sb->grps=(uint32_t)ceil(fours[1]*1.0/fours[8]);
			sb->totblocks=fours[1];
			sb->totinodes=fours[0];
			sb->inodesgrp=fours[10];
			sb->blocskgrp=fours[8];
			printf("Number of blocks groups= %d\n", sb->grps);
			printf("Number of blocks per group= %d\n", fours[8]);
			printf("Number of inodes per group= %d\n", fours[10]);
		}
		else{
			fprintf(stderr,"Mismatch number of block groups");
			fclose(img);
			return 1;
		}
		sb->blocksize=1024<<fours[6];
		printf("Block size=%d\n",1024<<fours[6]);
		printf("Major verison of FS=%d\n",fours[19]);
		if(fours[19]>=1){
			sb->inodesize=twos[44];
			sb->volname=(buffer+120);
			sb->firstinode=fours[21];
			sb->compression=(fours[24]%2==1);
			sb->typefield=((fours[24]>>1)%2==1);
			printf("Volume name= %s\n",sb->volname);
			printf("Required features=%d\n",fours[24]);
			printf("Compression algorithms used=%d\n", fours[50]);
			printf("First Inode=%d\n", fours[21]);
		}
		else{
			sb->inodesize=128;
			sb->firstinode=11;
		}
		printf("Inode size=%d\n",sb->inodesize);
		}
	else{
        perror("Error while reading Superblock");
        fclose(img);
        return 1;
		}
		
	unsigned char buffer2[sb->blocksize];

	bg=calloc(sb->grps,sizeof(struct BlockGroup));

	uint32_t seek=sb->blocksize;
	if (seek==1024)
		seek=2048;

	fseek(img, seek, SEEK_SET);

	if (fread(buffer2, 1, sb->blocksize, img) == sb->blocksize)
	{
		uint32_t* fours2=(uint32_t* )buffer2;
		uint16_t* twos2=(uint16_t* )buffer2;
		for(uint32_t i=0;i<sb->grps;i++){ 
			(bg+i)->blockmap=fours2[0];
			(bg+i)->inodemap=fours2[1];
			(bg+i)->inodetable=fours2[2];
			(bg+i)->unblocks=twos2[6];
			(bg+i)->uninodes=twos2[7];
			(bg+i)->dirs=twos2[9];
			printf("\n");
			printf("--------------------Block group %d -----------------\n", i+1);
			printf("Block address of block usage bitmap %d\n", fours2[0]);
			printf("Block address of inode usage bitmap %d\n", fours2[1]);
			printf("Starting block address of inode table %d\n", fours2[2]);
			printf("Number of unallocated blocks in group %d\n", twos2[6]);
			printf("Number of unallocated inodes in group %d\n", twos2[7]);
			printf("Number directories in group %d\n", twos2[8]);
			twos2+=16;
			fours2+=8;
		}
	}
	else{
		fprintf(stderr,"Unable to read block group descriptor table");
		fclose(img);
		return 1;
	}

	printf("\n");

	bpth=calloc(1,sizeof(struct Path));
	bpth->name=malloc(1);
	strcpy(bpth->name,"");
	epth=bpth;

	char cmd[512];
	cd=calloc(1,sizeof(struct Directory));
	cd->inodenumber=2;
	cd->offset=sb->blocksize*(bg+0)->inodetable+sb->inodesize;

	//-------------Input Loop--------------------
	while(true){
		struct Path* t=bpth;
		while(true){
			printf("%s/",t->name);
			if(t->child==NULL)
				break;
			t=t->child;
		}
		printf(">>");
		if(fgets(cmd, sizeof(cmd), stdin)!=NULL){
			cmd[strcspn(cmd,"\n")]='\0';
		}
		else{
		}
		if(strcmp(cmd,"exit")==0){
			break;
		}
		else if(strcmp(cmd, "ls")==0){
			List(img,cd);
		}
		else if(strcmp(cmd,"cls")==0 || strcmp(cmd, "clear")==0){
			#ifdef _WIN32
    			system("cls");
			#else
    			printf("\033[2J\033[H");
			#endif
		}
		else if(strncmp(cmd,"cd",2)==0){
			char* nme=strchr(cmd,' ');
			if(nme!=NULL){
				nme+=1;
				if(nme[0]!=0 && nme[0]!=32){
					ChangeDir(img, cd, nme);
				}
			}
			else
				printf("Change directory command cd <dir name>\n");
		}
		else if(strncmp(cmd,"cat",3)==0){
			char* nme=strchr(cmd,' ');
			if(nme!=NULL){
				nme+=1;
				if(nme[0]!=0 && nme[0]!=32){
					Concatenate(img, cd, nme);
				}
			}
			else
				printf("Concatenate command cat <file name>\n");
		}
		else if(strcmp(cmd,"help")==0){
			printf("Available commands:\n");
    		printf("  help                  Show this help message\n");
    		printf("  exit                  Exit the program\n");
    		printf("  ls                    List the contents of the current directory\n");
    		printf("  cls / clear           Clear the screen\n");
    		printf("  cd <directory name>   Change current directory to the given subdirectory\n");
    		printf("  cat <file name>       Display the contents of the specified file\n");
		}
		else{
			printf("Command not found\n");
		}
	}
    fclose(img);
    return 0;
}