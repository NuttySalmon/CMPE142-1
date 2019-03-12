#include <stdio.h>

int
main(int  argc, char *argv[])
{	
	int counter = 1;
	while (argv[counter] != NULL){
		printf("output: %s\n", argv[counter]);
		counter++;
	}
	return 0;
}
