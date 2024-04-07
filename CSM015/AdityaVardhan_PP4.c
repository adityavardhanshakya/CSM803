//Code Written by : Gur Saran Adhar 2-25-21
//Matrix-Vector multiplication in parallel
//This program multiplies mxn matrix A with nx1 vector B and gives result mx1 vector C
//Matrix A is scattered among parallel processes and vector B is broadcast to all processes
//Use MPI functions MPI_Bcast(); MPI_Scatter(); MPI_Gather() as suggested by annotation in the program
//Note: run this code using NO_OF_ROWS and NO_OF_COLUMNS which are evenly divisible by the number of processes (at specified at run time)
//Output shown is run with 4 processes
// <<<<<<<<<<<<<<<<<<<<<<<<<<< Aditya Vardhan _ 2303752 - MSC CS
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "../../../Program Files (x86)/Microsoft SDKs/MPI/Include/mpi.h"
#define NO_OF_ROWS 20 // m
#define NO_OF_COLUMNS 64 // n
void generate_A_random_Row(int v[], int n); // generate a random ('binary' vector of size n) representing a row(because it is easier to check result of multiplication)
void print_A_Row(int v[], int n); //print a row
void generate_A_random_Vec(int[], int n); // although the same functions can be used to generate rows and vector(just for better clarity different functions areset up
void print_A_Vec(int[], int n);
int main(int argc, char* argv[])
{
	int id, p, i, j;
	int A[NO_OF_ROWS][NO_OF_COLUMNS]; //mxn matrix data
	int* A_local; // segment of matrix each process will work on
	int OneRowX[NO_OF_COLUMNS];
	int B[NO_OF_COLUMNS]; // nx1 vector B
	int C[NO_OF_ROWS] = { 0 }; // result mx1 vector C
	int* C_local; //segment of result vector C each process will compute
	int slice; // slice is the piece of mxn matrix worked on by individual processes
	int SizeOf_Partial_result; //size of partial vector computed by individual process
	double start_time, end_time, elapsed_time; // just for time measurement
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	slice = (NO_OF_ROWS * NO_OF_COLUMNS) / p; //slice of Matrix A data worked on by one process
	SizeOf_Partial_result = (NO_OF_ROWS) / p; // size of partial result computed by one process
	//set-up local memories at 'each process' to work on
	A_local = (int*)malloc(sizeof(int) * slice); // a process receives and works only on the segment of A
	C_local = (int*)malloc(sizeof(int) * SizeOf_Partial_result); // set up memoryat each process to hold part of result vector
	//initialize mxn Matrix data (with random 0-1 vectors)
	if (id == 0)
	{
		printf("Full matrix A is :%d, number of processes :%d\n\n", (NO_OF_ROWS * NO_OF_COLUMNS), p);
		printf("number of rows in A: %d, number of columns in A :%d\n\n", NO_OF_ROWS, NO_OF_COLUMNS);
		printf("sub-matrix (A_local) worked on by each process: %d, size of partial result(C_local) from each process : % d\n\n", slice, SizeOf_Partial_result);
		printf("\n Matrix A \n\n");
		for (i = 0; i < NO_OF_ROWS; i++)
		{
			generate_A_random_Row(A[i], NO_OF_COLUMNS); // generate one row at a time
			print_A_Row(A[i], NO_OF_COLUMNS); // print the value of row
		}
		printf("\n vector B\n\n");
		generate_A_random_Vec(B, NO_OF_COLUMNS); // generate one vector for B
		print_A_Vec(B, NO_OF_COLUMNS); // print the value of vector B
		printf("\n");
	}// end of if (id ==0)
	//set-up timers
	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();
	//process 0 broadcast vector B to all processes
	MPI_Bcast(B, NO_OF_COLUMNS, MPI_INT, 0, MPI_COMM_WORLD); // 2. <<<<<<<<<<<<<<<<<<<<<<<<<<<< < insert MPI_Bcast to broadcast B to all processes
	//root process 0 scatter the matrix A to all processes
	MPI_Scatter(A, slice, MPI_INT, A_local, slice, MPI_INT, 0, MPI_COMM_WORLD); // 3. <<<<<<<<<<<<<<<<<<<<<<<<<<<<<insert MPI_Scatter to scatter A to all processes
	for (i = 0; i < SizeOf_Partial_result; i++)
	{
		for (j = 0; j < NO_OF_COLUMNS; j++)
		{
			//extract one row from the part of the matrix A_Local received and stick it into OneRowX
			OneRowX[j] = A_local[i * NO_OF_COLUMNS + j]; //4. <<<<< Complete function definition for function Multiply and insert call to function Multiply Band OneRowX
		}
		C_local[i] = Multiply(B, OneRowX, NO_OF_COLUMNS); //5. <<<<<< Assign value returned by function Multiply into result vector C_local on eachprocess
	}
	//Gather at process 0, the resulting vector C from individual processes results C_Local
	MPI_Gather(C_local, SizeOf_Partial_result, MPI_INT, C, SizeOf_Partial_result, MPI_INT, 0, MPI_COMM_WORLD); // 6. <<<<<<<<<<<<<<<<<<<< < insert MPI_Gather to gather C_Local to C at process 0
	//print result vector C
	if (id == 0)
		print_A_Vec(C, NO_OF_ROWS);
	end_time = MPI_Wtime();
	elapsed_time = (end_time - start_time);
	MPI_Finalize();
}
void generate_A_random_Row(int v[], int n) // generate a random binary vector -- first argument, of size--second argument
{
	int i;
	for (i = 0; i < n; i++)
		v[i] = rand() % 2;
}
void print_A_Row(int v[], int n) // print binary vector --first argument, of size--second argument
{
	int i;
	for (i = 0; i < n; i++)
		printf("%d ", v[i]);
	printf("\n");
}
void generate_A_random_Vec(int v[], int n) // generate a random binary vector -- first argument, of size--second argument
{
	int i;
	for (i = 0; i < n; i++)
		v[i] = rand() % 2;
}
void print_A_Vec(int v[], int n) // print vector --first argument, of size--second argument
{
	int i;
	for (i = 0; i < n; i++)
		printf("%d ", v[i]);
	printf("\n");
}
int Multiply(int x[], int y[], int n) // multiply pairwise elements of x[] and y[], both of size n, and return the sum
{
	int i; int sum = 0;
	for (i = 0; i < n; i++)
		sum = sum + x[i] * y[i];
	return(sum);
}