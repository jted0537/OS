#include <stdio.h>
#include <stdlib.h>
#include <syscall.c>

int main(int argc, char** argv) {
	int i, j;
	int argument[argc+5];
	for(i=1; i<argc; i++) {
		for(j=0; j<strlen(argv[i]); j++) {
			argument[i] *= 10;
			argument[i] += argv[i][j]-'0';
		}
	}
	//printf("\n\n\n%d %d %d %d\n\n", argument[1], argument[2], argument[3], argument[4]);
	printf("%d %d\n", fibonacci(argument[1]), max_of_four_int(argument[1], argument[2], argument[3], argument[4]));
	return 0;
}
