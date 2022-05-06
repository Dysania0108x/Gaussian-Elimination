#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <omp.h>

/* Program paras */
#define PRAN 2000  /* Max value of N */
int N;  /* Matrix size */
int thread_num;
/* Matrices and vectors */
double A[PRAN][PRAN], B[PRAN], X[PRAN];
/* A * X = B, solve for X */


void Gaussia_elimination();
void backs();
/* Set the program paras from the command-line arguments */
void paras(int argc, char **argv) 
{

  if (argc >= 3) {
    N = atoi(argv[1]);
	  thread_num = atoi(argv[2]);
  }
  else {
    printf("Usage: %s <matrix_dimension> <thread_num> \n",
           argv[0]);

    exit(0);
  }

}

/* Initialize A and B (and X to 0.0s) */
void initialize_inputs() {
  int row, col;

  printf("\nInitializing...\n");
  printf(" \n");
  for (col = 0; col < N; col++) {
    for (row = 0; row < N; row++) {
      A[row][col] = rand() / 32768.0;
    }
    B[col] = rand() / 32768.0;
    X[col] = 0.0;
  }

}

/* Print input matrices */
void print_inputs() {
  int i,j;
	printf("Displaying Initial Matrix.\n");
	for (i=0;i<N;i++)
	{
		printf("| ");
		for(j=0;j<N; j++)
		{	
			printf("%lf ",A[i][j]);
		}
		printf("| | %lf |\n",B[i]);
	}
}

void printAnswer()
{
	int i;
	printf("\nSolution array x:\n\n");
	for (i=0;i<N;i++)
	{
		printf("|%lf|\n", X[i]);
	}	
}

int main(int argc, char **argv) {

  /* Process program paras */
	thread_num=1;
	paras(argc,argv);
	
  /* Initialize A and B */
  initialize_inputs();

  /* Print input matrices */
  //print_inputs();

  /* Start Clock */
  double starts=omp_get_wtime();
  Gaussia_elimination();
	backs();
  double ends=omp_get_wtime();
  
  /* Display output */
  //printAnswer();

  /* Display timing results */
  printf("------------------------------------");
  printf("\nexecute time: %lf s.\n",ends-starts);
  printf("------------------------------------------------------------------------");
  printf("\n");
  printf("\n");
  printf("\n");

  return(0);
}

void Gaussia_elimination() {
	
/*
	Due to the strong dependence on norm in the outermost loop of parallel, when the outer loop is parallel, 
	there will be competition in the calculation of multiplier. 
	So parallel inner loop

*/
 omp_set_num_threads(thread_num);
 int norm, row, col;
  for (norm = 0; norm < N - 1; norm++) {
    #pragma omp parallel for shared(A, B) private(norm) 
    for ( row = norm + 1; row < N; row++) {
     float multiplier = A[row][norm] / A[norm][norm];
      for (col = norm; col < N; col++) {
	         A[row][col] -= A[norm][col] * multiplier;
      }
      B[row] -= B[norm] * multiplier;
    }
  }

}
void backs(){
	
	/* Back substitution */
  int row, col;
  for (row = N - 1; row >= 0; row--) {
    X[row] =B[row];
    for (col = N-1; col > row; col--) {
      X[row] -= A[row][col] * X[col];
    }
    X[row] /= A[row][row];
  }
}
