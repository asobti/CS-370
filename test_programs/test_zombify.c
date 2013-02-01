#include<stdio.h>
#include<linux/unistd.h>
#include<errno.h>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Pass target PID argument");
		return 1;
	}

	long target = atol(argv[1]);
	
	if(syscall(290, target) == 0) {
		printf("Zombified.\n");
	} else {
		printf("Failed to zombify.\n");
	}
	
	return 0;
}
