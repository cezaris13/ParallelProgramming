#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <math.h>


//===== Globalus kintamieji ===================================================

int num_points = 9000;     // Tasku skaicius (max 50000). Didinant ilgeja matricos skaiciavimo ir sprendinio paieskos laikas
int num_variables = 3;      // Tasku, kuriuos reikia rasti, skaicius
int num_iterations = 13000;  // Sprendinio paieskos algoritmo iteraciju skaicius (didinant - ilgeja sprendinio paieskos laikas)


double **points;            // Masyvas taskams saugoti
double **distance_matrix;   // Masyvas atstumu matricai saugoti

//===== Funkciju antrastes ====================================================

double get_time();                                          // Funkcija laiko matavimui
void load_data();                                           // Duomenu ikelimo is failo funkcija
double Haversine_distance(double, double, double, double);  // Atstumo skaiciavimo pagal Haversino formule funkcija
double distance_from_matrix(int, int);                      // Atstumo paemimo is atstumu matricos funkcija
void random_solution(int*);                                 // Atsitiktinio sprendinio generavimo funkcija
double evaluate_solution(int*);                             // Funkcija sprendinio tikslo funkcijos reiksmei ivertinti

//=============================================================================
// Papildomos funkcijos. SIU FUNKCIJU LYGIAGRETINTI NEREIKIA.
//=============================================================================

double get_time() {
   struct timeval laikas;
   gettimeofday(&laikas, NULL);
   double rez = (double)laikas.tv_sec+(double)laikas.tv_usec/1000000;
   return rez;
}

//=============================================================================

void load_data() {
	//----- Load demand points ------------------------------------------------
	FILE *f;
	f = fopen("../Data/lab_data.dat", "r");
	points = new double*[num_points];
	for (int i=0; i<num_points; i++) {
		points[i] = new double[2];
		fscanf(f, "%lf%lf", &points[i][0], &points[i][1]);
	}
	fclose(f);
}

//=============================================================================

double Haversine_distance(double lat1, double lon1, double lat2, double lon2) {
	double dlat = fabs(lat1 - lat2);
	double dlon = fabs(lon1 - lon2);
	double aa = pow((sin((double)dlat/(double)2*0.01745)),2) + cos(lat1*0.01745) *
               cos(lat2*0.01745) * pow((sin((double)dlon/(double)2*0.01745)),2);
	double c = 2 * atan2(sqrt(aa), sqrt(1-aa));
	double d = 6371 * c;
	return d;
}

//=============================================================================

double distance_from_matrix(int i, int j) {
	if (i >= j) return distance_matrix[i][j];
	else return distance_matrix[j][i];
}

//=============================================================================

void random_solution(int *solution) {
    bool unique;
	for (int i=0; i<num_variables; i++) {
		do {
			solution[i] = (int)((double)rand()/RAND_MAX * num_points);
			unique = 1;
            for (int j=0; j<i; j++)
				if (solution[j] == solution[i]) {
					unique = 0;
					break;
				}
		} while (unique == 0);
	}
}

//=============================================================================

double evaluate_solution(int *solution) {
	double distance, min_distance, total_distance = 0;
	for (int i=0; i<num_points; i++) {
        min_distance = 1e10;
        for (int j=0; j<num_variables; j++) {
		    distance = distance_from_matrix(i, solution[j]);
            if (distance < min_distance) {
                min_distance = distance;
            }
        }
        total_distance += min_distance;
    }
	return total_distance;
}
