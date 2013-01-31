#include<stdio.h>
#include<linux/unistd.h>
#include<errno.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Pass pid as argument");
		return 1;
	}

	long pid = atol(argv[1]);
	
	if (syscall(286, pid) == 0) {
		printf("Stolen\n");
	} else {
		printf("Failed\n");
	}
	
	return 0;
}
