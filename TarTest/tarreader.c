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
	file = malloc(sizeof(file));
	char* tarfile = "readme.tar";
	char* filecontents;
	long filesize;
	int tarfile_size;
	char* tarfile_contents;
	FILE* fh;
	int i, j;

	// read in the file into a buffer
	fh = fopen(tarfile, "r");

	fseek(fh, 0L, SEEK_END);
	filesize = ftell(fh);
	fseek(fh, 0L, SEEK_SET);

	filecontents = (char*)malloc(filesize + 1);		// +1 for null terminator

	fread(filecontents, filesize, 1, fh);
	filecontents[filesize] = 0;
	fclose(fh);
	
	// read in filename
	for (i = 0; i < 100; i++) {
		if (filecontents[i] != 0) {
			file->name[i] = filecontents[i];
		} else {
			break;
		}
	}
	// null terminate filename
	file->name[i] = 0;

	// read in owner's numeric id	
	for (i = 108; i < 116; i++) {
		file->uid[i-108] = filecontents[i];
	}
	file->uid[8] = 0;

	// read in group id
	for (i = 116; i < 124; i++) {
		file->gid[i-116] = filecontents[i];
	}
	file->gid[8] = 0;
	
	// read in file size
	for (i = 124; i < 136; i++) {
		file->size[i-124] = filecontents[i];
	}
	file->size[12] = 0;

	tarfile_size = octalStringToInt(file->size, 11);
	// printf("%d", tarfile_size);

	// read in file contents
	tarfile_contents = malloc(tarfile_size + 1);

	for (i = 512; i < 512 + tarfile_size; i++) {
		tarfile_contents[i - 512] = filecontents[i];
	}

	tarfile_contents[tarfile_size] = 0;

	printf("File name: %s\n", file->name);
	printf("UID: %d\n", octalStringToInt(file->uid, 7));
	printf("GID: %d\n", octalStringToInt(file->gid, 7));
	printf("File size: %d\n", tarfile_size);
	printf("File contents: \n");
	printf("%s", tarfile_contents);

	free(file);
	free(filecontents);
	free(tarfile_contents);
}