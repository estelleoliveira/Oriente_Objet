#ifndef PROBLEM_H
#define PROBLEM_H
#include "IMesh.h"
#include "Equation.h"
#include "Variable.h"

#include <memory>
#include <vector>
#include <iostream>


class Problem {
public : 
    Problem(std::shared_ptr<IMesh> mesh,int NbIter) : _mesh(std::move(mesh)), NombreIterations(NbIter) {}   //constructeur avec shared_ptr
    Problem(const Problem&) = delete;
    Problem& operator=(const Problem&) = delete;
    
    void solve();
    bool has_converged(std::shared_ptr<Variable> u_k, std::shared_ptr<Variable> u_kp1, double epsilon);
    void solve_jacobi(std::function<double (double, double, double)> function);
    void solve_gauss_seidel(std::function<double (double, double, double)> function);
    void solve_jacobi_gauss_seidel(std::function<double (double, double, double)> function);

private : 
    std::shared_ptr<IMesh> _mesh; //pointeur vers le maillage
    Equation _equation; //instance de la classe équation
    int NombreIterations;
};

#endif //PROBLEM_H