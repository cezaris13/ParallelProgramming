#include <stdio.h>
#include <omp.h>

int main() {
    omp_set_num_threads(2);
    printf("seq");
    # pragma omp parallel {
    // printf("")
    //
    printf("par");
    }

    printf("seq");

}
