#ifndef JACOBI_H
#define JACOBI_H

#include <vector>
#include <iomanip> //pour setprecision
#include "IMesh.h"

class Jacobi {
public:
    static void compute(std::shared_ptr<Variable> u_k, std::shared_ptr<Variable> u_kp1, const IMesh* mesh) {
    //std::cout << "Computing Jacobi iteration..." << std::endl;

    for (int i = 1; i < mesh->nb_points() - 1; ++i) {
        (*u_kp1)[i] = ((*u_k)[i-1] + (*u_k)[i+1]) / 2.0;  //calcul de u_i^{k+1} en utilisant la formule de Jacobi
    }
    
    #ifdef DEBUG_MODE
    for (int i = 0; i < u_kp1->size(); ++i) {
        std::cout << "u_k[" << i << "] = " << (*u_k)[i] << std::fixed << std::setprecision(5) << ",\t\t u_kp1[" << i << "] = " << std::fixed << std::setprecision(5) << (*u_kp1)[i] << std::endl;
    }
    #endif
    }
};

#endif //JACOBI_H