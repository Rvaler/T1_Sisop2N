#include <stdio.h>
#include <stdlib.h>

int numberRowsMatrixOne, numberColsMatrixOne, numberRowsMatrixTwo, numberColsMatrixTwo;
int numberOfProcesses; // indicated in the call

int main(int argc, char *argv[]){

	if (argv[1] == NULL){
		printf("Error - Must indicate the number of processes in terminal line \n");
		return -1;
	}
	sscanf(argv[1], "%d", &numberOfProcesses);
	printf("processos %i\n", numberOfProcesses);
	FILE *inFileMatrixOne, *inFileMatrixTwo, *outputFileResultMatrix;

	//reading matrix files and output creation
	inFileMatrixOne = fopen("in1.txt", "r");
	inFileMatrixTwo = fopen("in2.txt", "r");
	outputFileResultMatrix = fopen("out.txt", "w");

	if( !inFileMatrixOne || !inFileMatrixTwo || !outputFileResultMatrix ){
		printf("\nError in text files management.\n");
		return -1;
	}

	fscanf(inFileMatrixOne, "LINHAS = %d\nCOLUNAS = %d", &numberRowsMatrixOne, &numberColsMatrixOne);
	fscanf(inFileMatrixTwo, "LINHAS = %d\nCOLUNAS = %d", &numberRowsMatrixTwo, &numberColsMatrixTwo);

	if(numberColsMatrixOne != numberRowsMatrixTwo){
		printf("Error - Number of matrix one columns and matrix two lines must be equal.\n");
		return -1;
	}

	printf("linhamatriz %d", numberRowsMatrixOne);
	printf("linhamatriz %d", numberColsMatrixOne);
	printf("linhamatriz %d", numberRowsMatrixTwo);
	printf("linhamatriz %d", numberColsMatrixTwo);

	


	return 0;
}
