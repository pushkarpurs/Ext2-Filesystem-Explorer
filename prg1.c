#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 1024

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

    unsigned char buffer[BLOCK_SIZE];
    int block_num = 1;
	
	if (fseek(img, block_num * BLOCK_SIZE, SEEK_SET) != 0) {
        perror("Failed to seek to block");
        return 1;
    }

    if (fread(buffer, 1, BLOCK_SIZE, img) == BLOCK_SIZE) {
		
        // printf("Block %d:\n", block_num++);
        // for (int i = 0; i < BLOCK_SIZE; i++) {
            // printf("%02x ", buffer[i]);
            // if ((i + 1) % 16 == 0) printf("\n");
        // }
		if(buffer[56]==83 && buffer[57]==239)
			printf("The image is in Ext2 format");
		else{
			fprintf(stderr, "Image is nhot in Ext2 format");
			return 1;
		}
		}
	else{
        perror("Error while reading");
        fclose(img);
        return 1;
		}

    fclose(img);
    return 0;
}
