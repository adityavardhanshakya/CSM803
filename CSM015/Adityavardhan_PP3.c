//Code Written by : Gur Saran Adhar 2-16-21
//Illustrates the use of MPI functions MPI_Reduce(); MPI_Scatter(); MPI_Gather()
//Implements Genetic Operations 'CrossOver', and 'Mutation' in Parallel
//Modified by:
//<<<<------------- Aditya Vardhan_2303752


#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

constexpr auto LENGTH_OF_CHROMOSOME = 32;

void generate_A_random_chromosome(int v[], int n); // generate a random ('binary' vector of size n) representing a chromosome
void print_A_chromosome(int v[], int n); //print a chromosome
void Mutate(int y[], int len); // mutate:  array--first argument; length --second argument; 


int main(int argc, char* argv[])
{
	int id, p;
	int Z[LENGTH_OF_CHROMOSOME];  //One chromosomes data
	int* Z_local;  // segment of chromosomes each process will work on


	int slice;   // slice is the piece of Chromosome manipulated by individual processes 
	double start_time, end_time, elapsed_time; // just for time measurement

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	slice = (LENGTH_OF_CHROMOSOME) / p;  //slice of data worked on by a process 

	//set-up local memories at 'each process' to work on
	Z_local = (int*)malloc(sizeof(int) * slice);  // a process receives and works only on the segment of Z

	//initialize Chromosomes data
	generate_A_random_chromosome(Z, LENGTH_OF_CHROMOSOME); // generate one initial chromosome

	if (id == 0)
	{
		generate_A_random_chromosome(Z, slice); // generate one initial chromosome 
		printf("\n BEFORE GENETIC MANIPULATION\n\n");
		print_A_chromosome(Z, LENGTH_OF_CHROMOSOME);  // print the initial value of chromosome
		printf("\n");
	}// end of if (id ==0)

	//set-up  timers
	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();

	// process 0 scatters the  data Z among multiple processes in the communicator
	//<<<< insert call to MPI_Scatter()

	MPI_Scatter(Z, slice, MPI_INT, Z_local, slice, MPI_INT, 0, MPI_COMM_WORLD);

	srand(id);  //seed the random number for each process with process rank
	//<<<<< Complete function definition for function Mutate below and insert call to function Mutate

	Mutate(Z_local, slice);

	//.....collect at process 0, the resulting chromosomes from mutation 
	//<<<<< insert call to MPI_Gather()

	MPI_Gather(Z_local, slice, MPI_INT, Z, slice, MPI_INT, 0, MPI_COMM_WORLD);

	end_time = MPI_Wtime();
	elapsed_time = (end_time - start_time);

	if (id == 0)
	{   // print chromosome after genetic manipulation
		printf("\n AFTER GENETIC MUTATION\n\n");
		print_A_chromosome(Z, LENGTH_OF_CHROMOSOME);
		printf("\n");
	}
	MPI_Finalize();
}


void generate_A_random_chromosome(int v[], int n)  // generate a random binary vector --first argument, of size--second argument
{
	int i;
	for (i = 0; i < n; i++)
		v[i] = rand() % 2;
}

void print_A_chromosome(int v[], int n)  // print binary vector --first argument, of size--second argument
{
	int i;
	for (i = 0; i < n; i++)
		printf("%d  ", v[i]);
	printf("\n");
}

void Mutate(int y[], int len)  // '1-point' mutation
{
	int index; //array index of element to flip (mutate)
	//generate a random index to mutate (flip) within [0..len] 
	index = rand() % (len + 1);
	y[index] = 1 - y[index];
}

