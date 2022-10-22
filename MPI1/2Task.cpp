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
        int arr[10]={0,1,2,3,4,5,6,7,8,9};
        for(int i=0;i<10;i++){
            buff= arr[i];
    		MPI_Send(&buff, 1, MPI_INT, (i%(proc_count-1)+1),i, MPI_COMM_WORLD);
            MPI_Recv(&buff, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
            arr[stat.MPI_TAG]=buff;
        }
        for(int i = 1; i<proc_count;i++){
    		MPI_Send(&buff, 1, MPI_INT, i, 25, MPI_COMM_WORLD);
        }
        for(int i=0;i<10;i++){
            cout<<arr[i]<<" ";
        }
        cout<<endl;
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
