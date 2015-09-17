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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

FILE *inFileMatrixOne, *inFileMatrixTwo, *outputFileResultMatrix;
int *matrixOne, *matrixTwo, *outputMatrix;
int numberRowsMatrixOne, numberColsMatrixOne, numberRowsMatrixTwo, numberColsMatrixTwo;
int numberOfThreads; // indicated in the call

void loadMatrixValues(void);
void saveResults(void);
void *apllyMatrixMultiplication(void *row);
void multiplyElement(int elementRow, int elementCol);

int main(int argc, char *argv[]){

	if (argv[1] == NULL){
		printf("Error - Must indicate the number of threads in terminal line \n");
		return -1;
	}
	sscanf(argv[1], "%d", &numberOfThreads);
	if (numberOfThreads <= 0){
		printf("Error - number of threads must be greater than 0. \n");
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

	int i;
	pthread_t threadsArray[numberOfThreads];
	for (i = 0; i < numberOfThreads; i++){
		int *threadNumber = malloc(sizeof(int));
		if (threadNumber != NULL){
			*threadNumber = i;
			pthread_t th;
			pthread_create(&th, NULL, apllyMatrixMultiplication, (void *)threadNumber);
			threadsArray[i] = th;	
		}else{
			printf("Could not alloc threadNumber\n");
		}
	}


	int t;
	for (t = 0; t < numberOfThreads; t++){
		pthread_join(threadsArray[t],NULL); 
	}

	printf("acabou\n");
	saveResults();
}


void *apllyMatrixMultiplication(void *row){
	int currentRowMatrixOne = *(int *) row;
	while(currentRowMatrixOne < numberRowsMatrixOne){
		int currentColMatrixTwo;
		for (currentColMatrixTwo = 0; currentColMatrixTwo < numberColsMatrixTwo; currentColMatrixTwo++){
			multiplyElement (currentRowMatrixOne, currentColMatrixTwo);
		}
		currentRowMatrixOne++;
	}
	printf("thread %i acabando\n", *(int *)row);
}

void multiplyElement(int elementRow, int elementCol){
	int i, j, value = 0;
	for(i = 0; i < numberColsMatrixOne; i++){
		value += matrixOne[elementRow*numberColsMatrixOne + i] * 
				 matrixTwo[i*numberColsMatrixTwo + elementCol];
	}
	outputMatrix[elementRow*numberColsMatrixTwo + elementCol] = value;
}

void saveResults(){
	
	fprintf(outputFileResultMatrix, "LINHAS = %d\nCOLUNAS = %d\n", numberRowsMatrixOne, numberColsMatrixTwo);
	int i, j;
	for (i = 0; i < numberRowsMatrixOne; i++){
		for (j = 0; j < numberColsMatrixTwo; j++){
			printf("%i\n", outputMatrix[i*numberColsMatrixTwo + j]);
			fprintf(outputFileResultMatrix, "%d ", outputMatrix[i*numberColsMatrixTwo + j]);
		}
		fprintf(outputFileResultMatrix, "\n");
	}
}


// creating and filling matrixes
void loadMatrixValues(){

	matrixOne = malloc(sizeof(*matrixOne) * numberRowsMatrixOne * numberColsMatrixOne);
	matrixTwo = malloc(sizeof(*matrixTwo) * numberRowsMatrixTwo * numberColsMatrixTwo);
	outputMatrix = malloc(sizeof(int *) * numberRowsMatrixOne * numberColsMatrixTwo);

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