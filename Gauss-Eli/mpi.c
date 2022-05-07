#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

/* Program paras */
#define MAXN 2000  /* Max value of N */
int N;  /* Matrix size */
int myid = 0, comm_sz = 0;
/* Matrices and vectors */
int i;
int j;
/* A * X = B, solve for X */

	MPI_Status status;
/* Prototype */
void Gaussia_elimination(double *A,double *B,double *X,int M,int N);  
void backs(double *X,double *A,double *B,int M,int N);

/* Set the program paras from the command-line arguments */
void paras(int argc, char **argv) 
{

  if (argc >= 2) {
    N = atoi(argv[1]);
   
    if (N < 1 || N > MAXN) {
      printf("N = %i is out of range.\n", N);
      exit(0);
    }
  }
  else {
    printf("Usage: %s <matrix_dimension> <thread_num> \n",
           argv[0]);
    exit(0);
  }

}

/* Initialize A and B (and X to 0.0s) */
void initialize_inputs(double *A,double *B,double *X,int N) {
  int i,j;

  printf("\nInitializing input matrix...\n");
  for (j = 0; j < N; j++) {
    for (i = 0; i < N; i++) {
      A[i*N+j] = rand() / 32768.0;
      //A[i*N+j] = rand();
    }
    B[j] = rand() / 32768.0;
     //B[j] = rand();
    X[j] = 0.0;
  }

}

/* Print input matrices */
void print_inputs(double *A,double *B) {
  //int i,j;
	printf("Displaying Initial Matrix.\n");
	for (i=0;i<N;i++)
	{
		printf("| ");
		for(j=0;j<N; j++)
		{	
			printf("%lf ",A[i*N+j]);
		}
		printf("| | %lf |\n",B[i]);
	}
}

void printAnswer(double *X,int N)
{
	//int i;
	printf("\nSolution Vector (x):\n\n");
	for (i=0;i<N;i++)
	{
		printf("|%lf|\n", X[i]);
	}	
}


int factor(int n) {
	// return the largest factor that is not larger than sqrt(n)
	double sqr_root = sqrt(n);
  //int i;
	for (i = sqr_root; i >= 1; i--) {
		if (n % i == 0) return i;
	}
}
int main(int argc, char **argv) {
	double *A,*B,*X;
	double t1,t2;
  MPI_Init(&argc,&argv);//Initiating MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
		
  /* Process program paras */
  paras(argc, argv);
  /*
  
  In distributed parallel computing, the master-slave mode is one of the classical modes, 
  so the program chooses this mode to realize parallel computing
    
  */
  /* comm_sz equal is 1: serial execute */
  t1=MPI_Wtime();
if(comm_sz == 1){
	A = (double*)malloc(N * N * sizeof(double));
		B = (double*)malloc(N * sizeof(double));
		X= (double*)malloc(N  * sizeof(double));
  /* Initialize A and B */
  initialize_inputs(A,B,X,N);
  
  //serial compute
  Gaussia_elimination(A,B,X,N,N);
  backs(X,A,B,N,N);
  t2=MPI_Wtime();

		
	
  /* Print input matrices */
//  print_inputs();
//printAnswer(X,N);

		free(A);
		free(B);
		free(X);
}
else
{
	/* many procs execute*/
	
	int saveN = N;//  keep origial N value
	//block N by comm_sz
	if (N % (comm_sz - 1) != 0) {
			N-= N % (comm_sz - 1);
			N += (comm_sz - 1);
	}
	//get last block
		int addnum = N - saveN;
		//compute x and y position length
		int a = (comm_sz - 1) / factor(comm_sz - 1);
		int b = factor(comm_sz - 1);
		//compute row  and column length of block
		int each_row = N / a;
		int each_column = N / b;
		int n=N;//update length of N
		// use master and slave method for parallel compute
/* send data and recv result by  master proc*/
		if (myid == 0) {
			A = (double*)malloc(n * n * sizeof(double));
			B = (double*)malloc(n * sizeof(double));
			X= (double*)malloc(n  * sizeof(double));
		
		initialize_inputs(A,B,X,n);
		//int i; int j;
		for (i = 1; i < comm_sz; i++)
			{
				int beginRow = ((i - 1) / b) * each_row;
				int beginColumn = ((i - 1) % b) * each_column;

				// A: beginRow ~ endRow
				MPI_Send(&A[beginRow * n], each_row * n, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
				//  首地址                  字节数           类型    目标  消息标识符  通信域
				// B: n times, once a row
				for (j = 0; j < n; j++)
				{
					MPI_Send(&B[j], 1, MPI_DOUBLE, i, j + comm_sz, MPI_COMM_WORLD);
				}
			}
			
			for (i = 1; i < comm_sz; i++) {
				int beginRow = ((i - 1) / b) * each_row;
				int endRow = beginRow + each_row;
				int beginColumn = ((i - 1) % b) * each_column;


				for (j = beginRow; j < endRow; j++)
				{
					MPI_Recv(&X[j ], 1, MPI_DOUBLE, i,  2000, MPI_COMM_WORLD, &status);
				}


			}
			t2=MPI_Wtime();
			 //printAnswer(X,saveN);
			free(A);
		free(B);
		free(X);
				
		}
		else {
			//recv A and B ,then parallel compute by other proc,and send result of C to master  
			double *partA, *partB, *partC;
			partA = (double*)malloc(each_row * n * sizeof(double));
			partB = (double*)malloc(n * sizeof(double));
			partC = (double*)malloc(each_row * sizeof(double));

			// Recv: partA, partB
			MPI_Recv(&partA[0 * n + 0], each_row * n, MPI_DOUBLE, 0, myid, MPI_COMM_WORLD, &status);
			
			for (j = 0; j < n; j++)
			{	
				MPI_Recv(&partB[j], 1, MPI_DOUBLE, 0, j + comm_sz, MPI_COMM_WORLD, &status);
			}
			Gaussia_elimination(partA,partB,partC,each_row,n);
			 backs(partC,partA,partB,each_row,N);
			// Send: partC
			for (j = 0; j < each_row; j++) {
				MPI_Send(&partC[j], 1, MPI_DOUBLE, 0,  2000, MPI_COMM_WORLD);
			}
			free(partA);
		free(partB);
		free(partC);
			
		}
		
		
}
if(myid==0){
		printf("\nexecute time: %lf\n",t2-t1);
	}
MPI_Finalize(); //Finalizing the MPI
  	return 0;
}

/* 
 The elimination method is to express the unknowns of one equation in the system of equations with an algebraic formula containing another unknowns and substitute it into another equation, which eliminates an unknowns and obtains a solution; Or multiply one equation in the equation group by a constant and add it to another equation to eliminate an unknown number. The elimination method is mainly used to solve binary first-order equations.
core
1) The two equations are interchangeable and the solution is unchanged;
2) One equation is multiplied by non-zero number k, and the solution remains unchanged;
3) When one equation is multiplied by the number k plus another equation, the solution remains unchanged
 */
void Gaussia_elimination(double *A,double *B,double *X,int M,int N) {
  int norm, i,j;  /* Normalization row, and zeroing element row and col */
  double multip;

 
 
  for (norm = 0; norm < N - 1; norm++) {
    for (i = norm + 1; i < M; i++) {
      multip = A[i*N+norm] / A[norm*N+norm];
  
   for (j = norm; j < N; j++) {
	         A[row*N+j] -= A[norm*N+j] * multip;
      }
	  
      B[i] -= B[norm] * multip;
    }
  }
 
  
}
void backs(double *X,double *A,double *B,int M,int N){
	
	/* Back substitution */
  int i,j;
  for (i = M - 1; i >= 0; i--) {
    X[i] =B[i];
    for (j = N-1; j> i; j--) {
      X[i] -= A[i*N+j] * X[j];
    }
    X[i] /= A[i*N+i];
  }
}

