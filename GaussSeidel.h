#ifndef GAUSSSEIDEL_H
#define GAUSSSEIDEL_H

#include <vector>
#include <iomanip> //pour setprecision
#include "IMesh.h"

class GaussSeidel {
public:
    static void compute(std::shared_ptr<Variable> u_k_gs, std::shared_ptr<Variable> u_kp1_gs, const IMesh* mesh) {
    //std::cout << "Computing Gauss Seidel iteration..." << std::endl;

    for (int i = 1; i < mesh->nb_points() - 1; ++i) {
        (*u_kp1_gs)[i] = ((*u_kp1_gs)[i-1] + (*u_k_gs)[i+1]) / 2.0;  //calcul de u_i^{k+1} en utilisant la formule de Gauss Seidel
    }
    
    #ifdef DEBUG_MODE
    for (int i = 0; i < u_kp1_gs->size(); ++i) {
        std::cout << "u_k[" << i << "] = " << (*u_k_gs)[i] << std::fixed << std::setprecision(5) << ",\t\t u_kp1[" << i << "] = " << std::fixed << std::setprecision(5) << (*u_kp1_gs)[i] << std::endl;
    }
    #endif
    }
};

#endif //GAUSSSEIDEL_H