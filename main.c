#include <math.h>
#include "common.h"
#include "cat.h"
#include "cd.h"
#include "ls.h"

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