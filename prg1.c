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
	}typedef SuperBlock;

struct BlockGroup{
		uint32_t blockmap;
		uint32_t inodemap;
		uint32_t inodetable;
		uint16_t unblocks;
		uint16_t uninodes;
		uint16_t dirs;
	}typedef BlockGroup;

struct Directories{
		uint32_t InodeNumber;
		char* Name;
		struct Directories* parent;
		uint64_t offset;
	}typedef Directories;


void ReadContents(uint32_t blkno){
	printf("ReadingContentsFrom%d\n",blkno);
	return;
}

void List(FILE* img, struct Directories* dirptr){
	unsigned char ibuf[256];
	fseek(img, dirptr->offset, SEEK_SET);
	if(fread(ibuf,1,256,img)==256){
		uint32_t* fours=(uint32_t*)ibuf;
		fours+=10;
		for(int i=0;i<12;i++){
			if(fours[i]!=0){
				ReadContents(fours[i]);
			}
			else
				break;
		}
	}else{
		printf("Failed to read %s dir inode\n");
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

    FILE *img = fopen(argv[1], "rb");
    if (!img) {
        perror("Failed to open image file");
        return 1;
    }

	struct SuperBlock sb;

    unsigned char buffer[1024];
    int block_num = 1;
	
	if (fseek(img, block_num * 1024, SEEK_SET) != 0) {
        perror("Failed to seek to block");
        return 1;
    }

    if (fread(buffer, 1, 1024, img) == 1024) {
		printf("------------------SuprBlock------------------\n");
		if(buffer[56]==83 && buffer[57]==239)
			printf("The image is in Ext2 format\n");
		else{
			fprintf(stderr, "Image is nhot in Ext2 format");
			return 1;
		}
		uint32_t* fours=(uint32_t* )buffer;
		uint16_t* twos=(uint16_t* )buffer; 
		sb.fstate=twos[29];
		if(twos[29]==1)
		{
			printf("File system state is clean\n");
		}
		else{
			printf("File system state isnt clean\n");
		}
		if(ceil(fours[1]*1.0/fours[8])==ceil(fours[0]*1.0/fours[10])){
			sb.grps=(int)ceil(fours[1]*1.0/fours[8]);
			sb.totblocks=fours[1];
			sb.totinodes=fours[0];
			sb.inodesgrp=fours[10];
			sb.blocskgrp=fours[8];
			printf("Number of blocks groups= %d\n", (int)ceil(fours[1]*1.0/fours[8]));
			printf("Number of blocks per group= %d\n", fours[8]);
			printf("Number of inodes per group= %d\n", fours[10]);
		}
		else{
			printf("Mismatch number of block groups");
		}
		sb.blocksize=1024<<fours[6];
		printf("Block size=%d\n",1024<<fours[6]);
		printf("Major verison of FS=%d\n",fours[19]);
		if(fours[19]>=1){
			sb.inodesize=twos[44];
			sb.volname=(buffer+120);
			sb.firstinode=fours[21];
			printf("Volume name= %s\n",sb.volname);
			printf("Required features=%d\n",fours[24]);
			printf("Compression algorithms used=%d\n", fours[50]);
			printf("First Inode=%d\n", fours[21]);
		}
		else{
			sb.inodesize=128;
			sb.firstinode=11;
		}
		printf("Inode size=%d\n",sb.inodesize);
		}
	else{
        perror("Error while reading Superblock");
        fclose(img);
        return 1;
		}
		
	unsigned char buffer2[sb.blocksize];

	struct BlockGroup bg[sb.grps];

	int seek=sb.blocksize;
	if (seek==1024)
		seek=2048;

	fseek(img, seek, SEEK_SET);

	if (fread(buffer2, 1, sb.blocksize, img) == sb.blocksize)
	{
		uint32_t* fours2=(uint32_t* )buffer2;
		uint16_t* twos2=(uint16_t* )buffer2;
		for(int i=0;i<sb.grps;i++){ 
			bg[i].blockmap=fours2[0];
			bg[i].inodemap=fours2[1];
			bg[i].inodetable=fours2[2];
			bg[i].unblocks=twos2[6];
			bg[i].uninodes=twos2[7];
			bg[i].dirs=twos2[9];
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
		return 1;
	}

	printf("\n");

	char cmd[257];
	struct Directories dirs={2,"(root)",NULL,sb.blocksize*bg[0].inodetable+sb.inodesize};
	struct Directories* dirptr=&dirs;

	//-------------Input Loop--------------------
	while(true){
		struct Directories* t=dirptr;
		while(true){
			printf("%s/",t->Name);
			if (t->parent==NULL){
				printf(">>");
				break;
			}
			t=t->parent;
		}
		if(fgets(cmd, sizeof(cmd), stdin)!=NULL){
			cmd[strcspn(cmd,"\n")]='\0';
		}
		else{
		}
		if(strcmp(cmd,"exit")==0){
			break;
		}
		else if(strcmp(cmd, "ls")==0){
			List(img,dirptr);
		}
		else if(strcmp(cmd,"cls")==0){
			#ifdef _WIN32
    			system("cls");
			#else
    			printf("\033[2J\033[H");
			#endif
		}
	}

    fclose(img);
    return 0;
}