#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int main(int argc, char* argv[]) {

	// ------------------------------
	//      Declare variables 
	// ------------------------------

	struct tarfile *file;
	char* filename;
	
	char* filecontents;
	long filesize;
	int tarfile_size;

	// array to hold files
	struct tarfile* files[10];

	// file handler
	FILE* fh;

	// loop counters
	int i, j;

	int byteCount;
	int filecount = 0;

	// offset in tar file
	int offset = 0;	

	// ------------------------------
	//  End of variable declaration
	// ------------------------------

	// ------------------------------
	//     Check input argument
	// ------------------------------

	if (argc < 2) {
		printf("Please pass in path to tarfile as first argument.\n");
		printf("Exiting.\n");
		return 1;
	} else {
		filename = strdup(argv[1]);
	}

	// ------------------------------
	//     End of input arg check
	// ------------------------------


	// ------------------------------
	//     Read in the tarfile
	// ------------------------------

	// read in the file into a buffer
	fh = fopen(filename, "r");

	if (fh == NULL) {
		printf("Unable to open %s.\n", filename);
		printf("Exiting.\n");
		return 1;
	}

	fseek(fh, 0L, SEEK_END);
	filesize = ftell(fh);
	fseek(fh, 0L, SEEK_SET);
	
	filecontents = (char*)malloc(filesize + 1);		// +1 for null terminator

	byteCount = fread(filecontents, 1, filesize, fh);
	filecontents[filesize] = 0;	
	
	fclose(fh);

	if (byteCount != filesize) {
		printf("Error reading %s.\n", filename);
		printf("Expected filesize: %ld bytes.\n", filesize);
		printf("Bytes read: %d bytes.\n", byteCount);
		printf("Exiting.\n");
		// return 1;
	}
	// finished reading tar file and closed file handler

	// ------------------------------
	// Finished reading tar file and
	// closed file handle
	// ------------------------------


	// ------------------------------
	// Decode the contents of the tarfile
	// and store it in our array of struct
	// ------------------------------
	
	while(1) {

		// end of archive when filename begins with 0
		if (filecontents[offset] == 0) {
			printf("End of archive.\n");
			break;			
		}

		files[filecount] = (struct tarfile*) malloc(sizeof(struct tarfile*));

		// read in filename
		for (i = 0 + offset; i < 100 + offset; i++) {
			if (filecontents[i]) {				
				files[filecount]->name[i - (offset)] = filecontents[i];
			} else
				break;
		}

		// null terminate filename
		files[filecount]->name[i - offset] = 0;
		
		// read in owner's numeric id	
		for (i = 108 + offset; i < 116 + offset; i++) {
			files[filecount]->uid[i-(108 + offset)] = filecontents[i];
		}
		// null terminate
		files[filecount]->uid[8] = 0;

		// read in group id
		for (i = 116 + offset; i < 124 + offset; i++) {
			files[filecount]->gid[i-(116 + offset)] = filecontents[i];
		}
		// null terminate
		files[filecount]->gid[8] = 0;
		
		// read in file size
		for (i = 124 + offset; i < 136 + offset; i++) {
			files[filecount]->size[i-(124 + offset)] = filecontents[i];
		}
		// null terminate
		files[filecount]->size[12] = 0;
				
		tarfile_size = octalStringToInt(files[filecount]->size, 11);

		// store size of contents as an int
		files[filecount]->size_int = tarfile_size;

		// store the offset in the original tarfile at which this file's contents begin
		files[filecount]->contentOffset = 512 + offset;

		// update offset
		offset = (offset + ((tarfile_size/512) + 1) * 512) + 512;
		filecount++;
	}

	printf("%s\n", "------------------------------");
	printf("Finished reading %s.\n", filename);
	printf("%s\n", "------------------------------");

	// ------------------------------
	// Finished reading input tarfile
	// ------------------------------

	printf("Filecount: %d\n", filecount);

	// ------------------------------
	// List contents of the tarfile
	// ------------------------------

	printf("\n");

	printf("%s\n", "------------------------------");
	printf("Listing contents: \n");
	printf("%s\n", "------------------------------");

	for (j = 0; j < filecount; j++) {
		printf("File name: %s\n", files[j]->name);
		printf("UID: %d\n", octalStringToInt(files[j]->uid, 7));
		printf("GID: %d\n", octalStringToInt(files[j]->gid, 7));
		printf("File size: %d\n", octalStringToInt(files[j]->size, 11));
		printf("Byte offset: %d\n", files[j]->contentOffset);
		printf("\n");
	}

	printf("\n");



	// ------------------------------
	// Extract the files tar'ed
	// ------------------------------
	
	printf("%s\n", "------------------------------");
	printf("Extracting files: \n");
	printf("%s\n", "------------------------------");

	printf("\n");

	for (j = 0; j < filecount; j++) {
		FILE* fh;
		fh = fopen(files[j]->name, "w");
		byteCount = fwrite(&filecontents[files[j]->contentOffset], 1, files[j]->size_int, fh);

		if (byteCount == files[j]->size_int) {
			printf("Extracted file %s.\n", files[j]->name);
		}
	}

	printf("\nExtracted all files.\n");

	// ------------------------------
	// Finished extracting files
	// ------------------------------


	
	// ------------------------------
	// Free all malloc'ed memory
	// ------------------------------

	// according to gdb, this pointer is valid here
	// yet free'ing it throws an invalid pointer exception
	// free(filecontents);

	// free all mallocs
	for(j = 0; j < filecount; j++) {
		free(files[j]);
	}

	// ------------------------------
	//       AND WE'RE DONE...
	// ------------------------------

	return 0;
}