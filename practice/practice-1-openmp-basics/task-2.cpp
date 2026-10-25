#include <cstdio>
#include <omp.h>

int main() {
    omp_set_dynamic(0);
    omp_set_num_threads(8);
    #pragma omp parallel num_threads(4)
    {
        std::printf("Hello World!\n");
    }
}
