#include <iostream>
#include <mpi.h>

using namespace std;
int main(int argc, char *argv[]) {
    int size,rank,buff;
	MPI_Status stat;

	MPI_Init(&argc, &argv);
    MPI_Comm_size ( MPI_COMM_WORLD , &size );
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int proc_count= 2;
    if(rank == 0){
        int token=0;
        while(token<100){
            for(int i=1;i<proc_count;i++){
                MPI_Send(&token, 1, MPI_INT, (i%(proc_count-1)+1),i, MPI_COMM_WORLD);
            }
            for(int i=1;i<proc_count;i++){
                MPI_Recv(&token, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
            }
        }
        for(int i = 1; i<proc_count;i++){
    		MPI_Send(&buff, 1, MPI_INT, i, 25, MPI_COMM_WORLD);
        }
        cout<<token<<endl;
    }
    else{
        do{
            MPI_Recv(&buff, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
            if(stat.MPI_TAG!=25){
                int buff1 = buff +1;
                MPI_Send(&buff1, 1, MPI_INT, 0, stat.MPI_TAG, MPI_COMM_WORLD);
            }
        } while(stat.MPI_TAG!=25);
    }
	MPI_Finalize();
}
