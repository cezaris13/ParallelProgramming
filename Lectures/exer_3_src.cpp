#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

//=============================================================================

double GetTime() {
   struct timeval laikas;
   gettimeofday(&laikas, NULL);
   double rez = (double)laikas.tv_sec+(double)laikas.tv_usec/1000000;
   return rez;
}

//-----------------------------------------------------------------------------

void GenerateMatrix(int n, int m) {
   char filename[255]; sprintf(filename, "data_n%dm%d", n, m);
   FILE *f = fopen(filename, "w");
   for (int i=0; i<n; i++) {
      for (int j=0; j<m; j++) {
         //A[i*m+j] =
         fprintf(f, "%5d", (int)(1.0*rand()/RAND_MAX * 10));
      }
   }
   fclose(f);
   printf("File data_n%dm%d created.\n", n, m);
}

//-----------------------------------------------------------------------------

void LoadMatrix(int *A, int n, int m) {
   char filename[255]; sprintf(filename, "data_n%dm%d", n, m);
   FILE *f = fopen(filename, "r");
   for (int i=0; i<n*m; i++) fscanf(f, "%d", &A[i]);
   fclose(f);

}

//-----------------------------------------------------------------------------

void SortRow(int *A, int m) {
   int t;
   for (int i=0; i<m-1; i++)
      for (int j=0; j<m-1; j++)
         if (A[j] > A[j+1]) { t = A[j]; A[j] = A[j+1]; A[j+1] = t; }
}

//=============================================================================

int main() {
   srand(time(NULL));
   int n = 128000;         // Number of rows
   int m = 200;            // Number of columns
   int *A = new int[n*m];  // Matric stored in ONE-diemensional array
   double ts;

   GenerateMatrix(n, m);

   ts = GetTime();
   LoadMatrix(A, n, m);
   printf("Matrix loaded in %.2f\n", GetTime() - ts);

   // Sorting of rows of the matrix
   omp_set_num_threads(4);
   ts = GetTime();
# pragma omp parallel
   {
       int thread_num = omp_get_thread_num();
       int chunk = n/omp_get_num_threads();
       for (int i=thread_num*chunk; i<(thread_num+1)*chunk; i++) {
           SortRow(A+i*m, m);
       }
   }
   printf("Matrix sorted in %.2f\n", GetTime() - ts);
}
