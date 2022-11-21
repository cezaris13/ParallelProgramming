#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

using namespace std;

//===== Globalus kintamieji ===================================================

int num_points = 12000;     // Tasku skaicius (max 50000). Didinant ilgeja matricos skaiciavimo ir sprendinio paieskos laikas
int num_variables = 3;      // Tasku, kuriuos reikia rasti, skaicius
int num_iterations = 30000;  // Sprendinio paieskos algoritmo iteraciju skaicius (didinant - ilgeja sprendinio paieskos laikas)

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

int main(int argc, char *argv[]) {
    int world_rank, world_size;
    int buff;
    MPI_Status stat;
    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	double t_0 = get_time();    // Programos vykdymo pradzios laiko fiksavimas

    load_data();                // Duomenu ikelimas is failo

	double t_1 = get_time();    // Duomenu ikelimo pabaigos laiko fiksavimas

    //-------------------------------------------------------------------------
	// Skaiciuojam atstumu matrica
    // Matrica yra "trikampe", nes atstumai nuo A iki B ir nuo B iki A yra lygus
    //-------------------------------------------------------------------------

	distance_matrix = new double*[num_points];
    for(int i=0;i<num_points;i++){
        distance_matrix[i] = new double[i+1];
    }

    if(world_rank == 0){
        for (int i=0; i<num_points; i=i+(world_size-1)) {
            for(int j=1;j<world_size;j++){
                int temp = i + j - 1;
                MPI_Send(&temp, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
            }

            for(int j=1;j<world_size;j++){
                MPI_Recv(distance_matrix[i+j-1], i+1, MPI_DOUBLE, j, MPI_ANY_TAG, MPI_COMM_WORLD,&stat);
            }
        }
        for(int j=1;j<world_size;j++){
            MPI_Send(&buff, 1, MPI_INT, j, 25, MPI_COMM_WORLD);
        }
    }
    else{
        do{
            MPI_Recv(&buff, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
            if(stat.MPI_TAG!=25){
                double *localDistances = new double[buff + 1];
                for (int j=0; j<=buff; j++) {
                    localDistances[j] = Haversine_distance(points[buff][0], points[buff][1], points[j][0], points[j][1]);
                }
                MPI_Send(localDistances, buff + 1, MPI_DOUBLE, 0, stat.MPI_TAG, MPI_COMM_WORLD);
            }
        } while(stat.MPI_TAG!=25);

    }

    MPI_Barrier(MPI_COMM_WORLD);

    for(int i =0;i<num_points;i++){
        MPI_Bcast(distance_matrix[i],i+1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    for(int i=0;i<num_points;i++){
        for(int j= 0; j<=i;j++){
            printf("%d %f, ", world_rank, distance_matrix[i][j]);
        }
        printf("\n");
    }

	double t_2 = get_time();

    //-------------------------------------------------------------------------
	// Geriausio sprendinio pasieska paprastos atsitiktines paieskos algoritmu
    // (angl. Pure Random Search, PRS)
    //-------------------------------------------------------------------------

    int *solution = new int[num_variables];       // Masyvas atsitiktinai sugeneruotam sprendiniui saugoti
    int *best_solution = new int[num_variables];  // Masyvas geriausiam rastam sprendiniui saugoti
	double f_solution, f_best_solution = 1e10;     // Atsitiktinio ir geriausio rasto sprendiniu tikslo funkciju reiksmes

    MPI_Barrier(MPI_COMM_WORLD);

    if(world_rank == 0){
        for (int i=0; i<num_iterations; i=i+(world_size-1)) {
            for(int j=1;j<world_size;j++){
                MPI_Send(&buff, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
            }

            for(int j=1;j<world_size;j++){
                MPI_Recv(&buff, i+1, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD,&stat);
            }
        }
        for(int j=1;j<world_size;j++){
            MPI_Send(&buff, 1, MPI_INT, j, 12, MPI_COMM_WORLD);
        }
        int rankId = -1;
        for(int j=1;j<world_size;j++){
            double response;
            MPI_Recv(&response, 1, MPI_DOUBLE, j, 12, MPI_COMM_WORLD, &stat);
            if(response < f_best_solution) {
                rankId = j;
                f_best_solution = response;
            }
        }

        MPI_Send(&buff, 1, MPI_INT, rankId, 30, MPI_COMM_WORLD);
        MPI_Recv(best_solution, num_variables, MPI_DOUBLE, rankId, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

        for(int j=1;j<world_size;j++){
            MPI_Send(&buff, 1, MPI_INT, j, 25, MPI_COMM_WORLD);
        }
    }
    else {
        do{
            MPI_Recv(&buff, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
            if(stat.MPI_TAG!=25){
                if(stat.MPI_TAG == 0){
                    random_solution(solution);                  // Atsitiktinio sprendinio generavimas
                    f_solution = evaluate_solution(solution);   // Atsitiktinio sprendinio tikslo funkcijos skaiciavimas
                    if (f_solution < f_best_solution) {         // Tikrinam ar sugeneruotas sprendinys yra geresnis (mazesnis) uz geriausia zinoma
                        f_best_solution = f_solution;            // Jei taip, atnaujinam informacija apie geriausia zinoma sprendini
                        for (int j=0; j<num_variables; j++) {
                            best_solution[j] = solution[j];
                        }
                    }
                    MPI_Send(&buff, 1, MPI_INT, 0, stat.MPI_TAG, MPI_COMM_WORLD);
                }
                if(stat.MPI_TAG == 12) {
                    MPI_Send(&f_best_solution, 1, MPI_DOUBLE, 0, stat.MPI_TAG, MPI_COMM_WORLD);
                }
                if(stat.MPI_TAG == 30){
                    MPI_Send(best_solution, num_variables, MPI_INT, 0, stat.MPI_TAG, MPI_COMM_WORLD);
                }

            }
        } while(stat.MPI_TAG!=25);
    }

	double t_3 = get_time();    // Sprendinio paieskos pabaigos laiko fiksavimas

    if( world_rank == 0 ){
        cout<<"geriausia best solution reiksme: "<<f_best_solution<<endl;
        cout << "Geriausias rastas sprendinys (tasku indeksai duomenu masyve): ";
        for (int i=0; i<num_variables; i++) cout << best_solution[i] << "\t";
        cout << endl;
        cout << "Duomenu ikelimo laikas: " << t_1 - t_0 << " s." << endl;
        cout << "Atstumu matricos skaiciavimo laikas: " << t_2 - t_1 << " s." << endl;
        cout << "Sprendinio paieskos laikas: " << t_3 - t_2 << " s." << endl;
    }

    MPI_Finalize();
}

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
	f = fopen("lab_data.dat", "r");
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
	if (i >= j)	return distance_matrix[i][j];
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
