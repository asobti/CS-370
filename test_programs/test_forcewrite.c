#include<stdio.h>
#include<linux/unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Pass target files as first argument");
		return 1;
	}
	
	char* filename = argv[1];
	char* content = "Foobar";

	// here we open the file using READ_ONLY mode
	// this way even if the file is not writable, 
	// we can still open the file and get a file
	// descriptor for it
	int filedes = open(filename, O_RDONLY);
	
	if (filedes < 0) {
		printf("Failed to open file.\n");
	} else {

		// call to forcewrite syscall. Will write to the file even
		// though the file was opened in READ_ONLY mode
		if (syscall(289, filedes, content, strlen(content)) <= 0) {
			printf("Unable to write to file.\n");
		} else {
			printf("Wrote to file.\n");
		}
	}

	return 0;
}
