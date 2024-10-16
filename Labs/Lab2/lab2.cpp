#include <iostream>
#include <mpi.h>

#include "../Common/common.cpp"

#define GET_BEST_SOLUTION_VALUE 12
#define GET_BEST_SOLUTION 15
#define STOP_THREADS 17

using namespace std;

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int world_rank, world_size;
    int buff;
    MPI_Status stat;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    srand(time(NULL)*(world_rank+1));

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
                if(i+j <=num_points){
                    int temp = i + j - 1;
                    MPI_Send(&temp, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
                }
            }

            for(int j=1;j<world_size;j++){
                if(i+j <=num_points){
                    MPI_Recv(distance_matrix[i+j-1], i+j, MPI_DOUBLE, j, MPI_ANY_TAG, MPI_COMM_WORLD,&stat);
                }
            }
        }
        for(int j=1;j<world_size;j++){
            MPI_Send(&buff, 1, MPI_INT, j, STOP_THREADS, MPI_COMM_WORLD);
        }
    }
    else{
        do{
            MPI_Recv(&buff, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
            if(stat.MPI_TAG!=STOP_THREADS){
                double *localDistances = new double[buff + 1];
                for (int j=0; j<=buff; j++) {
                    localDistances[j] = Haversine_distance(points[buff][0], points[buff][1], points[j][0], points[j][1]);
                }
                MPI_Send(localDistances, buff + 1, MPI_DOUBLE, 0, stat.MPI_TAG, MPI_COMM_WORLD);
            }
        } while(stat.MPI_TAG!=STOP_THREADS);

    }

    MPI_Barrier(MPI_COMM_WORLD);

    for(int i =0;i<num_points;i++){
        MPI_Bcast(distance_matrix[i],i+1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // for(int i=0;i<num_points;i++){
    //     for(int j= 0; j<=i;j++){
    //         printf("%d %f, ", world_rank, distance_matrix[i][j]);
    //     }
    //     printf("\n");
    // }

	double t_2 = get_time();

    //-------------------------------------------------------------------------
	// Geriausio sprendinio pasieska paprastos atsitiktines paieskos algoritmu
    // (angl. Pure Random Search, PRS)
    //-------------------------------------------------------------------------
    int *solution = new int[num_variables];       // Masyvas atsitiktinai sugeneruotam sprendiniui saugoti
    int *best_solution = new int[num_variables];  // Masyvas geriausiam rastam sprendiniui saugoti
	double f_solution, f_best_solution = 1e10;     // Atsitiktinio ir geriausio rasto sprendiniu tikslo funkciju reiksmes

    MPI_Barrier(MPI_COMM_WORLD);
    int it = 1;
    int iterations = num_iterations/ world_size;
    int remainder = num_iterations % world_size;
    iterations += remainder > world_rank ? 1 : 0;
    for (int i=0; i<iterations; i++) {
		random_solution(solution);                  // Atsitiktinio sprendinio generavimas
		f_solution = evaluate_solution(solution);  // Atsitiktinio sprendinio tikslo funkcijos skaiciavimas
		if (f_solution < f_best_solution) {         // Tikrinam ar sugeneruotas sprendinys yra geresnis (mazesnis) uz geriausia zinoma
			f_best_solution = f_solution;            // Jei taip, atnaujinam informacija apie geriausia zinoma sprendini
			for (int j=0; j<num_variables; j++) {
                best_solution[j] = solution[j];
            }
		}
	}

    if(world_rank == 0){
        for(int j=1;j<world_size;j++){
            MPI_Send(&buff, 1, MPI_INT, j, GET_BEST_SOLUTION_VALUE, MPI_COMM_WORLD);
        }
        int rankId = -1;
        for(int j=1;j<world_size;j++){
            double response;
            MPI_Recv(&response, 1, MPI_DOUBLE, j, GET_BEST_SOLUTION_VALUE, MPI_COMM_WORLD, &stat);
            if(response < f_best_solution) {
                rankId = j;
                f_best_solution = response;
            }
        }

        if(rankId != 0 && rankId != -1){
            MPI_Send(&buff, 1, MPI_INT, rankId, GET_BEST_SOLUTION, MPI_COMM_WORLD);
            MPI_Recv(best_solution, num_variables, MPI_DOUBLE, rankId, GET_BEST_SOLUTION, MPI_COMM_WORLD, &stat);
        }

        for(int j=1;j<world_size;j++){
            MPI_Send(&buff, 1, MPI_INT, j, STOP_THREADS, MPI_COMM_WORLD);
        }
    }
    else {
        do{
            MPI_Recv(&buff, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
            if(stat.MPI_TAG!=STOP_THREADS){
                if(stat.MPI_TAG == GET_BEST_SOLUTION_VALUE) {
                    MPI_Send(&f_best_solution, 1, MPI_DOUBLE, 0, GET_BEST_SOLUTION_VALUE, MPI_COMM_WORLD);
                }
                if(stat.MPI_TAG == GET_BEST_SOLUTION){
                    MPI_Send(best_solution, num_variables, MPI_INT, 0, stat.MPI_TAG, MPI_COMM_WORLD);
                }
            }
        } while(stat.MPI_TAG!=STOP_THREADS);
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
