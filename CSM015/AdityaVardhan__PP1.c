#include <stdio.h>

/* We'll be using MPI routines, definitions, etc. */
#include "mpi.h"

void main(int argc, char** argv) {
    int         my_rank;   /* My process rank           */
    int         p;         /* The number of processes   */


    /* Let the system do what it needs to start up MPI */
    MPI_Init(&argc, &argv);

    /* Get my process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out how many processes there are */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) {
        /* Print number of processes */
        printf("no of procs = %d \n", p);

    }
    printf("Hello from ..... Aditya Vardhan .....Process = %d \n", my_rank);

    /* Shut down MPI */
    MPI_Finalize();
} /*  main  */
