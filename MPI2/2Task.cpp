#include <iostream>
#include <mpi.h>


using namespace std;
int main(int argc, char *argv[]) {

   MPI_Init(&argc, &argv);

   // Procesoriaus rangas pagrindiniame komunikatoriuje
   int world_rank, world_size;
   MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);

   int color;
   int chunkSize = world_size/2;
   color = world_rank / chunkSize;
   MPI_Comm comm;
   MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &comm);

   int comm_rank, comm_size;
   // Naujo komunikatoriaus dydis ir procesoriaus rangas jame
   MPI_Comm_rank(comm, &comm_rank);
   MPI_Comm_size(comm, &comm_size);

   for (int i=0; i<world_size; i++) {
       if (world_rank == i){
           printf("World size: %d \tWorld rank: %d \t commSize: %d \t commRank: %d \n", world_size,world_rank, comm_size, comm_rank);
       }
      MPI_Barrier(MPI_COMM_WORLD);
   }

   MPI_Finalize();
}
