#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <omp.h>

/* Program paras */
#define PRAN 2000  /* Max value of N */
int n;  /* Matrix size */
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
    n = atoi(argv[1]);
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
  int i,j;

  printf("\nInitializing input matrix...\n");
  printf(" \n");
  for (j = 0; j < n; j++) {
    for (i = 0; i < n; i++) {
      A[i][j] = rand() / 32768.0;
    }
    B[j] = rand() / 32768.0;
    X[j] = 0.0;
  }

}

/* Print input matrices */
void print_inputs() {
  int i,j;
	printf("Displaying Initial Matrix.\n");
	for (i=0;i<n;i++)
	{
		printf("| ");
		for(j=0;j<n; j++)
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
	for (i=0;i<n;i++)
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
 int norm, i, j;
  for (norm = 0; norm < n - 1; norm++) {
    #pragma omp parallel for shared(A, B) private(norm) 
    for ( i = norm + 1; i < n; i++) {
      float multip = A[i][norm] / A[norm][norm];
      for (j = norm; j < n; j++) {
           A[i][j] -= A[norm][j] * multip;
      }
      B[i] -= B[norm] * multip;
    }
  }
}
void backs(){
	
	/* Back substitution */
  int i, j;
  for (i = n - 1; i >= 0; i--) {
    X[i] =B[i];
    for (j = n-1; j > i; j--) {
      X[i] -= A[i][j] * X[j];
    }
    X[i] /= A[i][i];
  }
}
