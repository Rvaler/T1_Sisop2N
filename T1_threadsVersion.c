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