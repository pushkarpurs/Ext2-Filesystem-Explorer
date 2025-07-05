#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Provide image as argument");
        return 1;
    }

    FILE *img = fopen(argv[1], "rb");
    if (!img) {
        perror("Failed to open image file");
        return 1;
    }
	
	struct SuperBlock{
		uint32_t blocksize;
		uint16_t inodesize;
		uint32_t totblocks;
		uint32_t totinodes;
		uint32_t inodesgrp;
		uint32_t blocskgrp;
		uint32_t grps;
		uint16_t fstate;
		char* volname;
	}sb;

    unsigned char buffer[1024];
    int block_num = 1;
	
	if (fseek(img, block_num * 1024, SEEK_SET) != 0) {
        perror("Failed to seek to block");
        return 1;
    }

    if (fread(buffer, 1, 1024, img) == 1024) {
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
			printf("Volume name= %d, %d, %d, %d\n",fours[40],fours[41],fours[42],fours[43]);
			printf("Required features=%d\n",fours[24]);
			printf("Compression algorithms used=%d\n", fours[50]);
		}
		else
			sb.inodesize=128;
		printf("Inode size=%d\n",sb.inodesize);
		}
	else{
        perror("Error while reading Superblock");
        fclose(img);
        return 1;
		}
		
	unsigned char buffer2[sb.blocksize];

	struct BlockGroup{
		uint32_t blockmap;
		uint32_t inodemap;
		uint32_t inodetable;
		uint16_t unblocks;
		uint16_t uninodes;
		uint16_t dirs;
	};
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
			printf("Block group %d -----------------------------\n", i+1);
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
	
    fclose(img);
    return 0;
}