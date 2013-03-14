#include <stdio.h>
#include <stdlib.h>
#include "tarreader.h"

int octalStringToInt(char *string, unsigned int size){
  unsigned int output = 0;
  while(size > 0){
    output = output*8 + *string - '0';
    string++;
    size--;
  }
  return output;
}

void main() {
	struct tarfile *file;	

	char* tarfile = "readme.tar";
	
	char* filecontents;
	long filesize;
	int tarfile_size;

	// array to hold files
	struct tarfile* files[2];

	// initial malloc needs to be larger than any possible realloc we do
	// not sure why this is the case, but we're working around it now
	// ny doing an initial malloc with a large enough size
	// tarfile_contents = (char *)malloc(1024);
	
	// file handler
	FILE* fh;

	// loop countersP
	int i, j;

	// offset in tar file
	int offset = 0;	

	// read in the file into a buffer
	fh = fopen(tarfile, "r");

	fseek(fh, 0L, SEEK_END);
	filesize = ftell(fh);
	fseek(fh, 0L, SEEK_SET);
	
	filecontents = (char*)malloc(filesize + 1);		// +1 for null terminator

	fread(filecontents, filesize, 1, fh);
	filecontents[filesize] = 0;	
	
	fclose(fh);
	// finished reading tar file and closed file handler	
	
	for(j = 0; j < 2; j++) {

		files[j] = (struct tarfile*) malloc(sizeof(struct tarfile*));

		// read in filename
		for (i = 0 + offset; i < 100 + offset; i++) {
			if (filecontents[i]) {				
				files[j]->name[i - (offset)] = filecontents[i];
			} else
				break;
		}
		// null terminate
		files[j]->name[i - offset] = 0;
		
		// read in owner's numeric id	
		for (i = 108 + offset; i < 116 + offset; i++) {			
			files[j]->uid[i-(108 + offset)] = filecontents[i];
		}
		// null terminate
		files[j]->uid[8] = 0;

		// read in group id
		for (i = 116 + offset; i < 124 + offset; i++) {
			files[j]->gid[i-(116 + offset)] = filecontents[i];
		}
		// null terminate
		files[j]->gid[8] = 0;
		
		// read in file size
		for (i = 124 + offset; i < 136 + offset; i++) {
			files[j]->size[i-(124 + offset)] = filecontents[i];
		}
		// null terminate
		files[j]->size[12] = 0;

		tarfile_size = octalStringToInt(files[j]->size, 11);
		// files[j]->contents = (char*) malloc(tarfile_size + 1);
		
		// for (i = 512 + offset; i < 512 + tarfile_size + offset; i++) {
		// 	files[j]->contents[i - (512 + offset)] = filecontents[i];
		// }

		// // null terminate
		// files[j]->contents[tarfile_size] = 0;		

		// update offset
		offset = (offset + ((tarfile_size/512) + 1) * 512) + 512;
		//filesize -= offset;
		printf("New offset: %d\n", offset);
	}

	printf("%s\n", "---------------");
	printf("Finished reading tarfiles. Listing contents: \n");
	printf("%s\n", "---------------");

	for (j = 0; j < 2; j++) {
		printf("File name: %s\n", files[j]->name);
		// printf("UID Octal: %s\n", files[j]->uid);
		// printf("UID: %d\n", octalStringToInt(files[j]->uid, 7));
		printf("GID: %d\n", octalStringToInt(files[j]->gid, 7));
		printf("File size: %d\n", tarfile_size);
		printf("File contents: \n");
		printf("\n");		
	}
	
	
	free(filecontents);	
}