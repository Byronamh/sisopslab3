#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/syscall.h>



typedef struct{
	int f;
	int fila;
	int columna;
	int (*sudokuMatrix)[9];
	int nmap;
}parameters;



void *validateSubarreglo(void *);

void *checkRow(void *);

void *checkColumn(void *);

int readSudokuGrid(int (*grid)[9], int, FILE *);

#define NTHREADS 11

int main(int argc, char *argv[]){
	static int nmapi;

	if(argc < 2){
		exit(EXIT_FAILURE);
	}

	int sudokuMatrix[9][9];
	int n_grids;
	FILE *fp = fopen(argv[1], "r");
	fscanf(fp,"%d",&n_grids);
	fseek(fp, 1, SEEK_CUR); 

	parameters *data[9];
	int fila, columna, i = 0;
	for(fila = 0; fila < 9; fila += 3){
		for(columna = 0; columna < 9; columna += 3, ++i){

			data[i] = (parameters *)malloc(sizeof(parameters));

			if(data[i] == NULL){
				int err = errno;
				puts("malloc failed");
				puts(strerror(err));
				exit(EXIT_FAILURE);
			}

			data[i]->fila = fila;
			data[i]->columna = columna;
			data[i]->sudokuMatrix = sudokuMatrix;
		}
	}


	pthread_t tid[NTHREADS];

	int g, p, j, h, retCode, threadCheckFlag = 0, t_status[NTHREADS];

	for(g = 1; g <= n_grids; ++g){
		if(readSudokuGrid(sudokuMatrix, g, fp)){
			exit(EXIT_FAILURE);
		}

		for(p = 0; p < 9; ++p){
			if(retCode = pthread_create(&tid[p], NULL, validateSubarreglo, (void *)data[p])){
				exit(EXIT_FAILURE);
			}
		}

		if(retCode = pthread_create(&tid[9], NULL, checkRow, (void *)data[0])){
			exit(EXIT_FAILURE);
		}

		if(retCode = pthread_create(&tid[10], NULL, checkColumn, (void *)data[0])){
			exit(EXIT_FAILURE);
		}



		for(j = 0; j < NTHREADS; ++j){
			if(retCode = pthread_join(tid[j], (void *)&t_status[j])){
				exit(EXIT_FAILURE);
			}
		}

		for(h = 0; h < NTHREADS; ++h){
			if(t_status[h]){
				threadCheckFlag = 1;
				break;
			}
		}

		if(threadCheckFlag){
			printf(" Invalid\n", g);
			execlp("ps", "-p", "<#proc>", "lLf");
			syscall(SYS_gettid);
		}else{
			printf(" El thread que se ejecuta en el metodo para ejecutar el metodo de revision de columas es: %d\n", (int)getpid());
			printf(" El thread que se ejecuta en el main es: %d\n", (int)getpid()-1);
			printf(" Valid\n", g);
			execlp("ps", "-p", "<#proc>", "lLf");
		}
		threadCheckFlag = 0;
	}
	for(int k = 0; k < 9; ++k){
		free(data[k]);
	}

	fclose(fp);
	return 0;
}
void *validateSubarreglo(void *data){
	int validationVector[10] = {0};
	parameters *params = (parameters *)data;
	int i, j;
	printf(" En la revision de columnas el siguiente es un thread en ejecucion: %d\n", (int)getpid());
	for(i = params->fila; i < params->fila + 3; ++i){
		for(j = params->columna; j < params->columna + 3; ++j){
			if(validationVector[params->sudokuMatrix[i][j]] == 1){
				return (void *)-1;
			}
			validationVector[params->sudokuMatrix[i][j]] = 1;
		}
	}
	return (void *)0;
}
void *checkRow(void *data){
	int validationVector[10] = {0};

	parameters *params = (parameters *)data;

	int i, j;
	for(i = 0; i < 9; ++i){
		for(j = 0; j < 9; ++j){
			if(validationVector[params->sudokuMatrix[i][j]] == 1){
				return (void *)-1;
			}
			validationVector[params->sudokuMatrix[i][j]] = 1;
		}
		memset(validationVector, 0, sizeof(int)*10);
	}

	return (void *)0; 

}
void *checkColumn(void *data){
	int validationVector[10] = {0};

	parameters *params = (parameters *)data;

	int i, j;
	for(i = 0; i < 9; ++i){
		for(j = 0; j < 9; ++j){
			if(validationVector[params->sudokuMatrix[j][i]] == 1){
				return (void *)-1; 
			}
			validationVector[params->sudokuMatrix[j][i]] = 1;
		}
		memset(validationVector, 0, sizeof(int)*10);
	}
	return (void *)0; 
}
int readSudokuGrid(int (*grid)[9], int gridNumber, FILE *fp){
	int garbage;

	fseek(fp, 0, SEEK_SET);
	fscanf(fp, "%d", &garbage);
	fseek(fp, 1, SEEK_CUR); 

	if(gridNumber < 1){
		return -1;
	}

	else if(gridNumber > 1){
		fseek(fp, 9*(gridNumber - 1), SEEK_CUR);
	}
	fseek(fp, (gridNumber - 1)*sizeof(char)*81, SEEK_CUR);

	char entry;
	int i = 0, j = 0, totalValues = 0;
	while((fread(&entry, 1, 1, fp)) > 0 && totalValues < 81){ 
		if(entry != '\n'){ 
			if(isdigit(entry)){
				++totalValues;
				grid[i][j] = entry - '0'; 
				++j;
				if(j == 9){
					j = 0;
					++i;
				}
			}
			else{ 
				return -1;
			}
		}
	}
	
	return 0; 
}
int next() {
	int f;
	f = fork();
	if (f==0)
	{
		printf("%d\n", (int)getpid());
	}	
	else
	{
		printf("%d\n", (int)getpid());
		return 0;
	}
}