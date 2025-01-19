#include "Equation.h"
#include "Variable.h"
#include "IMesh.h"
#include <iostream>
#include <iomanip>  //pour setprecision
#include <chrono>
//#include <execution>
//#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>


Equation::Equation() {}
Equation::Equation(std::shared_ptr<IMesh> mesh, int iterations) {}

void Equation::compute(std::shared_ptr<Variable> u_k, std::shared_ptr<Variable> u_kp1, const IMesh* mesh) { //on a besoin du vecteur de u_k et u_kp1 et du maillage
    //std::cout << "Computing Jacobi iteration..." << std::endl;
    
    
    for (int i = 1; i < mesh->nb_points() - 1; ++i) {
        (*u_kp1)[i] = ((*u_k)[i-1] + (*u_k)[i+1]) / 2.0;  //calcul de u_i^{k+1} en utilisant la formule de Jacobi
    }
    /* Pour l'utilisation de STL pour la parallélisation, le programme ne fonctionne pas quand tbb installé, j'ai donc préféré ne pas l'utiliser*/
    /*std::vector<size_t> indices(mesh->nb_points() - 2);
    std::iota(indices.begin(), indices.end(), 1); // Fill with 1, 2, ..., nb_points-2
    
    std::for_each(std::execution::par, indices.begin(), indices.end(),
        [u_k, u_kp1](size_t i) {
            (*u_kp1)[i] = ((*u_k)[i-1] + (*u_k)[i+1]) / 2.0;
        }
    );*/
    #ifdef DEBUG_MODE
    for (int i = 0; i < u_kp1->size(); ++i) {
        std::cout << "u_k[" << i << "] = " << (*u_k)[i] << std::fixed << std::setprecision(5) << ",\t\t u_kp1[" << i << "] = " << std::fixed << std::setprecision(5) << (*u_kp1)[i] << std::endl;
    }
    #endif

}

/*L'initialisation permet de calculer u_k, on initialisait d'abord en deux zones (T1 dans la première partie du maillage et T2 dans la deuxième); on a ensuite modifié
pour pouvoir initialiser de deux façons, expliquées sur les fonctions après*/
void Equation::compute_initial_condition(Variable& u_k, const IMesh* _mesh, const std::function<double(double, double, double)>& initial_condition) {
    
    for (int i = 0; i < _mesh->nb_points(); ++i) {
        double x_i = _mesh->x_i(i);
        double x_min = _mesh->x_min();
        double x_max = _mesh->x_max();

        u_k[i] = initial_condition(x_i, x_min, x_max);  //permet d'utiliser l'initialisation voulue
    }
    /* Pour l'utilisation de STL pour la parallélisation, le programme ne fonctionne pas quand tbb installé, j'ai donc préféré ne pas l'utiliser*/
    /*std::vector<size_t> indices(_mesh->nb_points());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par, indices.begin(), indices.end(),
        [&u_k, _mesh, &initial_condition](size_t i) {
            double x_i = _mesh->x_i(i);
            double x_min = _mesh->x_min();
            double x_max = _mesh->x_max();
            u_k[i] = initial_condition(x_i, x_min, x_max);
        }
    );*/
}

/*Les conditions initiales sont générées et appelées depuis Problem::solve, ces deux fonctions ci-après permettent de définir deux types d'initialisation des valeurs, 
une initialisation deux zones (T1 jusqu'à la moitié des valeurs et T2 pour l'autre moitié), ou bien une initialisation uniforme (on fait (T1 + T2)/2) donc uniforme entre T1 et T2.*/
const std::function<double(double, double, double)>  two_zone_initialization = [](double x_i, double x_min, double x_max) {
    if (x_i < 0.5 * (x_min + x_max)) {return 30.0;} //T1 si x est un point de la première partie du maillage
    else {return 15.0;} //T2 sinon 
};
const std::function<double(double, double, double)>  uniform_initialization = [](double x_i, double x_min, double x_max) {
    return (30.0 + 15.0)/2.0;   //retourne (T1+T2)/2 dans tous les cas
};

/*Permet de déterminer les conditions limites de u_kp1 avant de déterminer les solutions à l'intérieur du vecteur.*/
void Equation::compute_boundary_conditions(Variable& u_kp1) {
    u_kp1[0] = 30.0;  //condition pour x=x_min 
    u_kp1[u_kp1.size() - 1] = 15.0;  //condition pour x=x_max
}

/*On calcule ici la solution exacte pour ensuite pouvoir comparer avec la solution calculée u_kp1*/
void Equation::compute_exact_solution(Variable& u_ref, IMesh* mesh) {
    int size = mesh->nb_points();
    double T1 = 30;
    double T2 = 15;

    for (int i = 0; i < size; ++i) {
        double x = mesh->x_i(i);
        u_ref[i] = (T2 - T1) * x + T1;
    }
    /* Pour l'utilisation de STL pour la parallélisation, le programme ne fonctionne pas quand tbb installé, j'ai donc préféré ne pas l'utiliser*/
    /*std::vector<size_t> indices(mesh->nb_points());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par, indices.begin(), indices.end(), [&u_ref, mesh, T1, T2](size_t i) {
        double x = mesh->x_i(i);
        u_ref[i] = (T2 - T1) * x + T1;
    });*/
}

/*Le résidu permet de savoir si la convergence a été atteinte suffisamment proche. Elle permet de comparer la précision de la méthode approximative et la comparer à la solution 
exacte qui elle a un résidu de 0.0.*/
double Equation::compute_residu(std::shared_ptr<Variable> u_k, const IMesh* mesh, double epsilon) {
    double residu = 0.0;
    
    for (size_t i = 1; i < u_k->size() - 1; ++i) {
        // Calcul du résidu pour le point i 
        double diff = ((*u_k)[i - 1] - 2 * (*u_k)[i] + (*u_k)[i + 1]);
        residu += diff * diff; //puissance 2
    }
    
    return std::sqrt(residu);
}


/*Export vers vtk pour visualiser vers Paraview*/
void Equation::export_to_vtk(std::shared_ptr<Variable> u_kp1, std::shared_ptr<Variable> u_ref, std::shared_ptr<Variable> u_k, const IMesh* mesh, std::string filename) {
    std::ofstream vtk_file(filename);

    //on crée et ouvre le fichier où l'on va noter toutes les données calculées
    if (!vtk_file.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier VTK." << std::endl;
        return;
    }

    //récupérer les informations du maillage
    int nb_points = mesh->nb_points();  //nombre total de points
    double dx = mesh->dx();  //pas de l'espace
    double x_min = mesh->x_min();  //position initiale

    //ENTETE du fichier VTK
    vtk_file << "# vtk DataFile Version 3.0\n";
    vtk_file << "Simulation Results\n";
    vtk_file << "ASCII\n";
    vtk_file << "DATASET STRUCTURED_GRID\n";
    vtk_file << "DIMENSIONS " << nb_points << " 1 1\n";
    vtk_file << "POINTS " << nb_points << " float\n";

    for (int i = 0; i < nb_points; ++i) {
        double x_i = mesh->x_i(i); 
        vtk_file << x_i << " 0.0 0.0\n";    //on exporte les positions de x 
    }

    vtk_file << "\nCELL_DATA " << nb_points - 1 << "\n";  //nombre de cellules = nombre de points - 1

    //exporter les données de `u_kp1` (température à l'itération suivante)
    vtk_file << "SCALARS u_kp1 float\n";    //légende 
    vtk_file << "LOOKUP_TABLE default\n";
    for (int i = 1; i < nb_points - 1; ++i) {
        vtk_file << (*u_kp1)[i] << "\n";    //on exporte u_kp1
    }

    //exporter les données de `u_ref` (solution exacte)
    vtk_file << "SCALARS u_ref float\n";
    vtk_file << "LOOKUP_TABLE default\n";
    for (int i = 0; i < nb_points; ++i) {
        vtk_file << (*u_ref)[i] << "\n";    //on exporte u_ref
    }

    //exporter les données de `u_k` (condition initiale)
    vtk_file << "SCALARS u_k float\n";
    vtk_file << "LOOKUP_TABLE default\n";
    for (int i = 0; i < nb_points; ++i) {
        vtk_file << (*u_k)[i] << "\n";      //on exporte u_k
    }

    vtk_file.close();   //on ferme le fichier ouvert 
    std::cout << "Données exportées vers le fichier '" << filename << "'." << std::endl;
}