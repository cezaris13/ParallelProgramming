#include <iostream>
#include <omp.h>

#include "../Common/common.cpp"

using namespace std;

int main() {

    srand(time(NULL));
    // srand(1);

    double t_0 = get_time();    // Programos vykdymo pradzios laiko fiksavimas

    load_data();                // Duomenu ikelimas is failo

    double t_1 = get_time();    // Duomenu ikelimo pabaigos laiko fiksavimas

    //-------------------------------------------------------------------------
    // Skaiciuojam atstumu matrica
    // Matrica yra "trikampe", nes atstumai nuo A iki B ir nuo B iki A yra lygus
    //-------------------------------------------------------------------------

    omp_set_num_threads(2);
    distance_matrix = new double *[num_points];

    // #pragma omp parallel for schedule(guided)
    for (int i = 0; i < num_points; i++) {
      distance_matrix[i] = new double[i + 1];
      for (int j = 0; j <= i; j++) {
        distance_matrix[i][j] = Haversine_distance(points[i][0], points[i][1],
                                                   points[j][0], points[j][1]);
      }
    }

    double t_2 = get_time(); // Matricos skaiciavimo pabaigos laiko fiksavimas

    //-------------------------------------------------------------------------
    // Geriausio sprendinio pasieska paprastos atsitiktines paieskos algoritmu
    // (angl. Pure Random Search, PRS)
    //-------------------------------------------------------------------------
    double f_best_solution = 1e10;
    int *best_solution =
        new int[num_variables]; // Masyvas geriausiam rastam sprendiniui saugoti
#pragma omp parallel reduction(min : f_best_solution)
    {
      int *best_solution_tmp =
          new int[num_variables]; // Masyvas geriausiam rastam sprendiniui
                                  // saugoti
      double f_solution,
          f_best_solution_tmp = 1e10; // Atsitiktinio ir geriausio rasto
                                      // sprendiniu tikslo funkciju reiksmes
#pragma omp for schedule(dynamic)
      for (int i = 0; i < num_iterations; i++) {
        int *solution =
            new int[num_variables]; // Masyvas atsitiktinai sugeneruotam
                                    // sprendiniui saugoti
        random_solution(solution);  // Atsitiktinio sprendinio generavimas
        f_solution = evaluate_solution(
            solution); // Atsitiktinio sprendinio tikslo funkcijos skaiciavimas
        if (f_solution <
            f_best_solution_tmp) { // Tikrinam ar sugeneruotas sprendinys yra
                                   // geresnis (mazesnis) uz geriausia zinoma
          f_best_solution_tmp = f_solution; // Jei taip, atnaujinam informacija
                                            // apie geriausia zinoma sprendini
          for (int j = 0; j < num_variables; j++) {
            best_solution_tmp[j] = solution[j];
          }
        }
      }
      f_best_solution = f_best_solution_tmp; // Jei taip, atnaujinam informacija
                                             // apie geriausia zinoma sprendini
#pragma omp barrier
      if (f_best_solution == f_best_solution_tmp) {
        for (int j = 0; j < num_variables; j++) {
          best_solution[j] = best_solution_tmp[j];
        }
      }
    }

    //  int *best_solution = new int[num_variables];  // Masyvas geriausiam
    //  rastam sprendiniui saugoti
    // double f_solution, f_best_solution = 1e10;     // Atsitiktinio ir
    // geriausio rasto sprendiniu tikslo funkciju reiksmes
    // #pragma omp parallel reduction (min: f_best_solution ) private
    // (f_solution) #pragma omp for schedule(dynamic) for (int i=0;
    // i<num_iterations; i++) {
    //     int *solution = new int[num_variables];       // Masyvas atsitiktinai
    //     sugeneruotam sprendiniui saugoti
    //     random_solution(solution);                  // Atsitiktinio
    //     sprendinio
    // generavimas 	f_solution = evaluate_solution(solution);   //
    // Atsitiktinio sprendinio tikslo funkcijos skaiciavimas if (f_solution <
    // f_best_solution) {         // Tikrinam ar sugeneruotas sprendinys yra
    // geresnis (mazesnis) uz geriausia zinoma  f_best_solution = f_solution;
    // // Jei taip, atnaujinam informacija apie geriausia zinoma sprendini
    //         if(f_best_solution == f_solution){
    //             #pragma omp critical (DataCollection)
    //             {
    //                 for (int j=0; j<num_variables; j++) {
    //                     best_solution[j] = solution[j];
    //                 }
    //             }
    //         }
    //     }
    // }

    double t_3 = get_time(); // Sprendinio paieskos pabaigos laiko fiksavimas

    //-------------------------------------------------------------------------
    // Rezultatų spausdinimas
    //-------------------------------------------------------------------------
    cout << "geriausia best solution reiksme: " << f_best_solution << endl;
    cout << "Geriausias rastas sprendinys (tasku indeksai duomenu masyve): ";
    for (int i = 0; i < num_variables; i++)
      cout << best_solution[i] << "\t";
    cout << endl;
    cout << "Duomenu ikelimo laikas: " << t_1 - t_0 << " s." << endl;
    cout << "Atstumu matricos skaiciavimo laikas: " << t_2 - t_1 << " s."
         << endl;
    cout << "Sprendinio paieskos laikas: " << t_3 - t_2 << " s." << endl;
}
