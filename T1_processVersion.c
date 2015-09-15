/*
TRABALHO 1 - SISOP 2 N

Rafael Valer - 220489
Rodrigo Boos - 
Luis Felipe Polo -

O trabalho consiste em implementar em C (não em C++) um programa concorrente para multiplicação de
matrizes. O programa deve ser implementado em duas versões diferentes, uma delas utilizando
processos UNIX e outra utilizando pthreads. Os programas deverão executar obrigatoriamente em
ambientes Unix (Linux) mesmo que tenham sido desenvolvidos sobre outras plataformas
*/



#include <string.h>
#include <unistd.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */


#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h> 
#include <sys/stat.h>

FILE *inFileMatrixOne, *inFileMatrixTwo, *outputFileResultMatrix;

int numberRowsMatrixOne, numberColsMatrixOne, numberRowsMatrixTwo, numberColsMatrixTwo;
int numberOfProcesses; // indicated in the call
int segmentId;
int *matrixOne, *matrixTwo, *outputMatrix;



void loadMatrixValues(void);
void printMatrix(void);
void apllyMatrixMultiplication(int row);
void multiplyAux(int row, int col);
void saveResults();
void multiplyElement(int elementRow, int elementCol);

int main(int argc, char *argv[]){

	if (argv[1] == NULL){
		printf("Error - Must indicate the number of processes in terminal line \n");
		return -1;
	}
	sscanf(argv[1], "%d", &numberOfProcesses);
	if (numberOfProcesses <= 0){
		printf("Error - number of processes must be greater than 0. \n");
		return -1;
	}

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

	loadMatrixValues();

	segmentId = shmget(IPC_PRIVATE, sizeof((*outputMatrix) * numberRowsMatrixOne * numberColsMatrixTwo), S_IRUSR | S_IWUSR);
	
	int childController = 0;
	int i;
	for (i = 0; i < numberOfProcesses; i++){
		pid_t forkPid;
		forkPid = fork();
		if(forkPid < 0){
			printf("\nError in forking process\n");
			return -1;

		}else if(forkPid == 0){ // child process
			printf("Im a child\n");
			apllyMatrixMultiplication(i);
			exit(0);
		}
	}
	// isso n da certo
	// if (childController == numberOfProcesses){
	// 	printf("ENTROU\n");
	// 	saveResults();
	// }

	int status, p;
	for (p = 0; p < numberOfProcesses; ++p)
		wait(&status);

	saveResults();

	// outputMatrix = (int *) shmat(segmentId, NULL, 0);
	// printMatrix();
	// shmdt(outputMatrix);
	//saveResults();
	return 0;
}

void saveResults(){
	outputMatrix = (int *) shmat(segmentId, NULL, 0);
	fprintf(outputFileResultMatrix, "LINHAS = %d\nCOLUNAS = %d\n", numberRowsMatrixOne, numberColsMatrixTwo);
	int i, j;
	for (i = 0; i < numberRowsMatrixOne; i++){
		for (j = 0; j < numberColsMatrixTwo; j++){
			printf("%i\n", outputMatrix[i*numberColsMatrixTwo + j]);
			fprintf(outputFileResultMatrix, "%d", outputMatrix[i*numberColsMatrixTwo + j]);
		}
		fprintf(outputFileResultMatrix, "\n");
	}
	shmdt(outputMatrix);
	//shmctl(segmentId, IPC_RMID, NULL);
}


void apllyMatrixMultiplication(int element){
	outputMatrix = (int*) shmat(segmentId, NULL, 0);

	while(element < numberRowsMatrixOne*numberColsMatrixTwo){
		int elementCol = element % numberColsMatrixTwo;
		int elementRow = element / numberRowsMatrixOne;
		multiplyElement(elementRow, elementCol);
		element += numberOfProcesses;
	}
	shmdt(outputMatrix);
}

void multiplyElement(int elementRow, int elementCol){
	int i, j, value = 0;
	for(i = 0; i < numberColsMatrixOne; i++){
		value += matrixOne[elementRow*numberColsMatrixOne + i] * 
				 matrixTwo[i*numberColsMatrixTwo + elementCol];
	}
	outputMatrix[elementRow*numberColsMatrixTwo + elementCol] = value;
}

// creating and filling matrixes
void loadMatrixValues(){

	matrixOne = malloc(sizeof(*matrixOne) * numberRowsMatrixOne * numberColsMatrixOne);
	matrixTwo = malloc(sizeof(*matrixTwo) * numberRowsMatrixTwo * numberColsMatrixTwo);

	int i, j;
	for(i = 0; i < numberRowsMatrixOne; i++){
		for(j = 0; j < numberColsMatrixOne; j++){
			fscanf(inFileMatrixOne, "%d", &matrixOne[i*numberColsMatrixOne + j]);
		}
	}
	for(i = 0; i < numberRowsMatrixTwo; i++){
		for(j = 0; j < numberColsMatrixTwo; j++){
			fscanf(inFileMatrixTwo, "%d", &matrixTwo[i*numberColsMatrixTwo + j]);
		}
	}
}

void printMatrix(){
	//PRINT OUTPUT MATRIX
	outputMatrix = (int*) shmat(segmentId, NULL, 0);
	int i, j;
	for(i = 0; i < numberRowsMatrixOne; i++){
		for(j = 0; j < numberColsMatrixTwo; j++){
			printf("%i\n", outputMatrix[i*numberColsMatrixTwo + j] );
		}
	}
	shmdt(outputMatrix);

	// PRINT MATRIX ONE
	// int i, j;
	// for(i = 0; i < numberRowsMatrixOne; i++){
	// 	for(j = 0; j < numberColsMatrixOne; j++){
	// 		printf("%d\n", matrixOne[i*numberColsMatrixOne + j] );
	// 	}
	// }

	// PRINT MATRIX TWO
	// for(i = 0; i < numberRowsMatrixTwo; i++){
	// 	for(j = 0; j < numberColsMatrixTwo; j++){
	// 		printf("%d\n", matrixTwo[i*numberColsMatrixTwo + j]);
	// 	}
	// }
}
