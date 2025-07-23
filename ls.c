#include "ls.h"

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