/*
TRABALHO 1 - SISOP 2 N

Rafael Valer - 220489
Rodrigo Boos - 221065
Luis Felipe Polo - 218327

O trabalho consiste em implementar em C (não em C++) um programa concorrente para multiplicação de
matrizes. O programa deve ser implementado em duas versões diferentes, uma delas utilizando
processos UNIX e outra utilizando pthreads. Os programas deverão executar obrigatoriamente em
ambientes Unix (Linux) mesmo que tenham sido desenvolvidos sobre outras plataformas
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h> 
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>



FILE *inFileMatrixOne, *inFileMatrixTwo, *outputFileResultMatrix;

int numberRowsMatrixOne, numberColsMatrixOne, numberRowsMatrixTwo, numberColsMatrixTwo;
int numberOfProcesses; // indicated in the call
int segmentId;
int *matrixOne, *matrixTwo, *outputMatrix;

void loadMatrixValues(void);
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
	segmentId = shmget(IPC_PRIVATE, (sizeof(int) * numberRowsMatrixOne * numberColsMatrixTwo), S_IRUSR | S_IWUSR);
	
	int i;
	for (i = 0; i < numberOfProcesses; i++){
		pid_t forkPid;
		forkPid = fork();
		if(forkPid < 0){
			printf("\nError in forking process\n");
			return -1;

		}else if(forkPid == 0){ // child process
			apllyMatrixMultiplication(i);
			exit(0);
		}
	}

	// wait for all processes to end execution
	int status, p;
	for (p = 0; p < numberOfProcesses; ++p)
		wait(&status);

	saveResults();
	return 0;
}



// save the output resulted matrix into the out.txt file
void saveResults(){
	outputMatrix = (int *) shmat(segmentId, NULL, 0);
	fprintf(outputFileResultMatrix, "LINHAS = %d\nCOLUNAS = %d\n", numberRowsMatrixOne, numberColsMatrixTwo);
	int i, j;
	for (i = 0; i < numberRowsMatrixOne; i++){
		for (j = 0; j < numberColsMatrixTwo; j++){
			fprintf(outputFileResultMatrix, "%d ", outputMatrix[i*numberColsMatrixTwo + j]);
		}
		fprintf(outputFileResultMatrix, "\n");
	}
	shmdt(outputMatrix);
	shmctl(segmentId, IPC_RMID, NULL);
}

// for each line of matrix one, a process will get the line and calculate its multiplication with the respective matrix two colunm
void apllyMatrixMultiplication(int currentRowMatrixOne){
	outputMatrix = (int*) shmat(segmentId, NULL, 0);

	while(currentRowMatrixOne < numberRowsMatrixOne){
		int currentColMatrixTwo;
		for (currentColMatrixTwo = 0; currentColMatrixTwo < numberColsMatrixTwo; currentColMatrixTwo++){
			multiplyElement (currentRowMatrixOne, currentColMatrixTwo);
		}
		currentRowMatrixOne += numberOfProcesses;
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

	matrixOne = (int *)malloc(sizeof(int) * numberRowsMatrixOne * numberColsMatrixOne);
	matrixTwo = (int *)malloc(sizeof(int) * numberRowsMatrixTwo * numberColsMatrixTwo);

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

