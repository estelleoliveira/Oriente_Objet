#ifndef EQUATION_H
#define EQUATION_H
#include "Variable.h"
#include "IMesh.h"
#include <functional>
#include <concepts>

extern const std::function<double(double, double, double)> two_zone_initialization;     //définition globale pour l'initialisation deux zones
extern const std::function<double(double, double, double)> uniform_initialization;      //définition globale pour l'initialisation uniforme

//définir un concept pour vérifier la méthode Solver::compute
template <typename Solver>
concept SolverWithCompute = requires(Solver, std::shared_ptr<Variable> u_k, std::shared_ptr<Variable> u_kp1, const IMesh* mesh) {
    { Solver::compute(u_k, u_kp1, mesh) } -> std::same_as<void>; // Vérifie le type de retour
};


class Equation {
public :
    Equation();
    Equation(std::shared_ptr<IMesh> mesh, int iterations);
    void compute(std::shared_ptr<Variable> u_k, std::shared_ptr<Variable> u_kp1, const IMesh* mesh);         //Equation::compute
    void compute_initial_condition(Variable& u_k, const IMesh* _mesh, const std::function<double(double, double, double)>& initial_condition); 
    void compute_boundary_conditions(Variable& u_kp1);
    void compute_exact_solution(Variable& u_ref, IMesh* mesh);
    double compute_residu(std::shared_ptr<Variable> u_k, const IMesh* mesh, double epsilon);
    void export_to_vtk(std::shared_ptr<Variable> u_kp1, std::shared_ptr<Variable> u_ref, std::shared_ptr<Variable> u_k, const IMesh* mesh, std::string filename);

    template <typename Solver>
    requires SolverWithCompute<Solver>
    void compute_for_solver(std::shared_ptr<Variable> u_k, std::shared_ptr<Variable> u_kp1, const IMesh* mesh) {
        Solver::compute(u_k, u_kp1, mesh);
    };

};


#endif //EQUATION_H