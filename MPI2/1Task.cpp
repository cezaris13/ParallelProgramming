#include <iostream>
#include <mpi.h>

#define N 8
using namespace std;

int main(int argc, char *argv[]) {
    double t1, t2;
    int id, numProcs;
    int *A = new int[N];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    if (id == 0) {
        for (int i=0; i<N; i++)
            A[i] = i;
    }

    if (id == 0) printf("Before:\n");
    for (int i=0; i<numProcs; i++) {
        if (id == i) {
            printf("Proc %d: ", id);
            for (int i=0; i<N; i++) printf("%d ", A[i]);
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    int chunkSize = N /numProcs;
    int *B = new int[N];

    MPI_Scatter(A, chunkSize, MPI_INT, B, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
    int sum = 0;

    for (int i=0; i<numProcs; i++) {
        if (id == i) {
            printf("Proc %d: ", id);
            for (int i=0; i<chunkSize; i++){
                printf("%d ", B[i]);
                sum+=B[i];
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    int *Response = new int[numProcs];

    MPI_Gather(&sum, 1, MPI_INT, Response, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(id == 0){
        printf("After:\n");
        int totalSum = 0;
        for(int i = 0;i<numProcs;i++){
            cout<<Response[i]<< " ";
            totalSum+=Response[i];
        }
        cout<<endl;
        printf("total sum: %d\n", totalSum);
    }
    //broadcast part

    MPI_Bcast(A, N, MPI_INT, 0, MPI_COMM_WORLD);
    int mindiff=10e7;
    int minVal=-1;
    for(int i=0;i<N;i++){
        int diff = abs(id-A[i]);
        if( diff < mindiff){
            mindiff = diff;
            minVal = A[i];
        }
    }

    for (int i=0; i<numProcs; i++) {
        if (id == i) {
            printf("Proc %d, minVal: %d\n", id, minVal);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
