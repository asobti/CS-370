#include<stdio.h>
#include<linux/unistd.h>
#include<errno.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Pass target pid as argument");
		return 1;
	}

	long pid = atol(argv[1]);
	long return_val = syscall(287, pid);

	if (syscall(291, pid) == 0) {
		printf("Task joined.\n");
	} else {
		printf("Failed\n");
	}
	
	return 0;
}
