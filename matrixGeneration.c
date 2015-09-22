#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char *argv[]){

	srand( (unsigned)time(NULL) );
	FILE *in1;
	int numberOfRows, numberOfCols;

	sscanf(argv[1], "%d", &numberOfRows);
	sscanf(argv[2], "%d", &numberOfCols);

	in1 = fopen("in1.txt", "w");
	
	fprintf(in1, "LINHAS = %d\nCOLUNAS = %d\n", numberOfRows, numberOfCols);
	int i, j;
	for (i = 0; i < numberOfRows; i++){
		for (j = 0; j < numberOfCols; j++){
			fprintf(in1, "%d ", rand() % 10);
		}
		fprintf(in1, "\n");
	}
}


