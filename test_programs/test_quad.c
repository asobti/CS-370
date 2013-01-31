#include<stdio.h>
#include<linux/unistd.h>
#include<errno.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Pass pid as argument");
		return 1;
	}

	long pid = atol(argv[1]);
	long return_val = syscall(287, pid);

	if (return_val > 0) {
		printf("Timeslice quadrupled. New timeslice: %ld.\n", return_val);
	} else {
		printf("Failed\n");
	}
	
	return 0;
}
