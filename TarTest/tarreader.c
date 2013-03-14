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
	char* tarfile_contents;

	// initial malloc needs to be larger than any possible realloc we do
	// not sure why this is the case, but we're working around it now
	// ny doing an initial malloc with a large enough size
	tarfile_contents = (char *)malloc(10240);
	
	// file handler
	FILE* fh;

	// loop counters
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
		file = (struct tarfile *)malloc(sizeof(file));

		// read in filename
		for (i = 0 + offset; i < 100 + offset; i++) {
			if (filecontents[i]) {				
				file->name[i - (offset)] = filecontents[i];
			} else
				break;
		}
		// null terminate
		file->name[i - offset] = 0;
		
		// read in owner's numeric id	
		for (i = 108 + offset; i < 116 + offset; i++) {			
			file->uid[i-(108 + offset)] = filecontents[i];
		}
		// null terminate
		file->uid[8] = 0;

		// read in group id
		for (i = 116 + offset; i < 124 + offset; i++) {
			file->gid[i-(116 + offset)] = filecontents[i];
		}
		// null terminate
		file->gid[8] = 0;
		
		// read in file size
		for (i = 124 + offset; i < 136 + offset; i++) {
			file->size[i-(124 + offset)] = filecontents[i];
		}
		// null terminate
		file->size[12] = 0;

		tarfile_size = octalStringToInt(file->size, 11);
		
		// read in file contents
		realloc(tarfile_contents, tarfile_size + 1);

		for (i = 512 + offset; i < 512 + tarfile_size + offset; i++) {
			tarfile_contents[i - (512 + offset)] = filecontents[i];
		}

		// null terminate
		tarfile_contents[tarfile_size] = 0;
		
		printf("File name: %s\n", file->name);
		// printf("UID Octal: %s\n", file->uid);
		// printf("UID: %d\n", octalStringToInt(file->uid, 7));
		printf("GID: %d\n", octalStringToInt(file->gid, 7));
		printf("File size: %d\n", tarfile_size);
		printf("File contents: \n");
		printf("%s", tarfile_contents);

		free(file);

		// update offset
		offset = (offset + ((tarfile_size/512) + 1) * 512) + 512;
		printf("New offset: %d\n", offset);
	}
	
	
	free(filecontents);
	free(tarfile_contents);	
}