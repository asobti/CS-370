#include<stdio.h>
#include<linux/unistd.h>
#include<errno.h>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Pass target PID and victim PID as arguments");
		return 1;
	}

	long target = atol(argv[1]);
	long victim = atol(argv[2]);
	
	long stolenTime = syscall(287, target, victim);
	
	printf("Stole a total of %ld time.\n", stolenTime);	
	
	return 0;
}
