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

void genMatrix(int *A, int N, int M) {
   // Išvalome matricą
   for (int i=0; i<N*M; i++) {
      A[i] = 0;
   }
   //----- Generuojam matricą -------------------------------------------------
   // Pirmas ketvirtadalis eilučių --    M/4 ilgio
   // Antras ketvirtadaslis eilučių --   M/2 ilgio
   // Trečias ketvirtadalis eilučių --   (M/4)*3 ilgio
   // Ketvirtas ketvirtadalis eilučių -- M ilgio
   //--------------------------------------------------------------------------
   int m = M/4;
   for (int i=0; i<N; i++) {
      for (int j=0; j<m; j++) A[i*M+j] = (int)(1.0*rand()/RAND_MAX*99) + 1;
      if (i > 0 && (i+1) % (N/4) == 0) m += M/4;
   }
}

//-----------------------------------------------------------------------------

double getMedian(int *A, int M) {
   // Randam eilutės elementų skaičių
   // Visi elementai yra teigiami, o likusi eilutės dalis -- nuliai
   int n = 0; while (n < M && A[n] != 0) n++;
   // Rikiuojam eilutės elementus didėjimo tvarka
   int t;
   for (int i=0; i<n-1; i++)
      for (int j=0; j<n-1; j++)
         if (A[j] > A[j+1]) { t = A[j]; A[j] = A[j+1]; A[j+1] = t; }
   // Grąžinam medianą
   if (n % 2 == 0) return 0.5*(A[n/2-1] + A[n/2]);
   else return A[n/2];
}

//-----------------------------------------------------------------------------

int main() {
   srand(time(NULL));
   int numRows = 256;                  // Eilučių skaičius 256
   int numCols = 10000;                // Stulpelių skaičius 10000
   int *M = new int[numRows*numCols];  // Matrica talpinama vienačiame masyve

   double t_start = GetTime();
   genMatrix(M, numRows, numCols);

   double t_load = GetTime();
   // for (int i=0; i<numRows; i++) {
   //    for (int j=0; j<numCols; j++) {
   //       printf("%5d", M[i*numCols+j]);
   //    }
   //    printf("\n");
   // }

   double avg=0;
   omp_set_num_threads(4);
#pragma omp parallel reduction (+:avg)
#pragma omp for schedule(guided)
   for (int i=0; i<numRows; i++) {
       avg = avg +getMedian(M+i*numCols, numCols);
   }
   printf("returned median average: %f\n", avg/numRows);
   double t_calc = GetTime();
   printf("Load time: %.2f\n", t_load - t_start);
   printf("Calc time: %.2f\n", t_calc - t_load);
}
