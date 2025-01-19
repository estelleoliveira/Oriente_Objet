#include "Problem.h"
#include "Variable.h"
#include "Equation.h"
#include "Jacobi.h"
#include "GaussSeidel.h"
#include "IMesh.h"
#include <iostream>
#include <chrono>
//#include <thread>
#include <future>

/*Ici, on commence par lancer la fonction de résolution Jacobi et de Gauss Seidel en parallèle. Puis on execute en séquentiel ces deux méthodes, 
d'abord avec une intialisation deux zones (T1 sur la premuère moitié du maillage puis T2 sur l'autre partie), puis celle avec l'intialisation 
uniforme ((T1+T2)/2).*/
void Problem::solve() {

    printf("Parallélisme : \n");
    std::future<void> JacobiThread = std::async(std::launch::async, [&](){  //async est un thread amélioré
        solve_jacobi(two_zone_initialization);  //intialisation deux zones
    });
    std::future<void> GaussSeidelThread = std::async(std::launch::async, [&](){
        solve_gauss_seidel(two_zone_initialization);
    });

    JacobiThread.wait();
    GaussSeidelThread.wait();

    printf("\n\nSéquentiel : \n");
    solve_jacobi_gauss_seidel(two_zone_initialization); //intitialisation deux zones

    printf("\nSéquentiel avec initialisation uniforme : \n");
    solve_jacobi_gauss_seidel(uniform_initialization);  //initialisation uniforme
    
}

void Problem::solve_jacobi(std::function<double (double, double, double)> function) {
    //global définition des variables
    auto u_k = std::make_shared<Variable>(_mesh, "u_k");
    auto u_kp1 = std::make_shared<Variable>(_mesh, "u_kp1");
    auto u_ref = std::make_shared<Variable>(_mesh, "u_ref");
    const double epsilon = 1e-5;  //seuil de convergence

    if (!_mesh) {
        std::cerr << "Erreur : Maillage nul. " << std::endl;    //affiche une erreur dans le cas où le maillage est null
        return;
    }

    //Jacobi résolution
    _equation.compute_exact_solution(*u_ref, _mesh.get());  //on résout la solution exacte
    _equation.compute_initial_condition(*u_k, _mesh.get(), function);   //on résout la solution initiale
    //utilisation de _mesh.get() pour obtenir un pointeur brut vers l'objet IMesh (avec utilisation de std::shared_ptr)
    u_k->print();   //on print les données calculées de la solution initiale dans le fichier de données de u_k
    _equation.compute_boundary_conditions(*u_kp1);  //définit les conditions aux limites avant de résoudre le problème

    std::chrono::time_point<std::chrono::system_clock> startJ, endJ; 
    startJ = std::chrono::system_clock::now();  //lance le timer pour déterminer les performances
    std::cout << "--- Solving Problem ---" << std::endl;    //affichage du début de résolution du problème
    for (int i = 0; i < NombreIterations; i++) {
        double residu = _equation.compute_residu(u_k, _mesh.get(), epsilon);
        if (has_converged(u_k, u_kp1, epsilon)) {
            std::cout << "Convergence atteinte pour iteration " << "\nRésidu Jacobi : " << std::fixed << std::setprecision(6) << residu << std::endl;  //renvoie l'itération pour laquelle on atteint la convergence
            break;
        }

        //std::cout << "--- Iterative method iteration : " << i + 1 << " ---" << std::endl;
        _equation.compute_for_solver<Jacobi>(u_k, u_kp1, _mesh.get());     //appel de Equation::compute via l'instance de équation, permet de lancer le calcul, ici on a mis le Solveur en template pour appeler la bonne méthode
        _equation.compute_boundary_conditions(*u_kp1);
        std::swap(u_k, u_kp1);

        if (i == NombreIterations - 1) {std::cout << "Reached maximum iterations without convergence ---" << std::endl;}
    }
    endJ = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_secondsJ = (endJ - startJ)*1000;
    u_kp1->print(); //print les données calculées de la solution finale dans le fichier de données de u_kp1
    u_ref->print(); //print les données calculées de la solution de exacte dans le fichier de données de u_ref
    std::cout << "Elapsed time Jacobi solve: " << elapsed_secondsJ.count() << "ms\n";  //affichage de la durée d'execution
    _equation.export_to_vtk(u_kp1, u_ref, u_k, _mesh.get(), "solution.vtk");
}

/*Le mécanisme est le meme pour gauss Seidel, les variables changent juste de nom*/
void Problem::solve_gauss_seidel(std::function<double (double, double, double)> function){
    //global définition
    auto u_k_gs = std::make_shared<Variable>(_mesh, "u_k_gs");
    auto u_kp1_gs = std::make_shared<Variable>(_mesh, "u_kp1_gs");
    auto u_ref_gs = std::make_shared<Variable>(_mesh, "u_ref_gs");
    const double epsilon = 1e-5;  //seuil de convergence

    if (!_mesh) {
        std::cerr << "Erreur : Maillage nul. " << std::endl;    //affiche une erreur dans le cas où le maillage est null
        return;
    }
    
    //Gauss Seidel résolution 
    _equation.compute_exact_solution(*u_ref_gs, _mesh.get());
    u_ref_gs->print();
    _equation.compute_initial_condition(*u_k_gs, _mesh.get(), function); //utilisation de _mesh.get() pour obtenir un pointeur brut ver l'objet IMesh (avec utilisation de std::shared_ptr)
    u_k_gs->print();
    _equation.compute_boundary_conditions(*u_kp1_gs);
    
    std::chrono::time_point<std::chrono::system_clock> startG, endG;
    startG = std::chrono::system_clock::now();
    std::cout << "--- Solving Problem ---" << std::endl;    //affichage du début de résolution du problème

    for (int i = 0; i < NombreIterations; i++) {
        double residu = _equation.compute_residu(u_k_gs, _mesh.get(), epsilon);
        if (has_converged(u_k_gs, u_kp1_gs, epsilon)) {
            std::cout << "Convergence atteinte pour iteration " << i << "\nRésidu GaussSeidel : " << std::fixed << std::setprecision(6) << residu << std::endl;  //renvoie l'itération pour laquelle on atteint la convergence
            break;
        }

        //std::cout << "--- Iterative method iteration : " << i + 1 << " ---" << std::endl;
        _equation.compute_for_solver<GaussSeidel>(u_k_gs, u_kp1_gs, _mesh.get());    //appel de Equation::compute via l'instance equation, permet de lancer le calcul
        _equation.compute_boundary_conditions(*u_kp1_gs);
        std::swap(u_k_gs, u_kp1_gs);

        if (i == NombreIterations - 1) {std::cout << "Reached maximum iterations without convergence ---" << std::endl;}
        
    }
    
    endG = std::chrono::system_clock::now();
    u_kp1_gs->print();
    
    std::chrono::duration<double> elapsed_secondsG = (endG - startG)*1000;
    std::cout << "Elapsed time Gauss Seidel solve: " << elapsed_secondsG.count() << "ms\n";

}

/*J'ai ici ajouté une fonction qui permet de résoudre le problème via les 2 methodes en séquentiel, on aurait pu utiliser std::async avec le paramètre deferred dans problem::solve, 
cependant j'ai voulu utiliser une fonction comme celle ci pour pouvoir calculer le temps total des deux méthodes qui ne concerne que la résolution et non pas l'initialisation des valeurs etc...*/
void Problem::solve_jacobi_gauss_seidel(std::function<double (double, double, double)> function) {
    //global définition
    auto u_k = std::make_shared<Variable>(_mesh, "u_k");
    auto u_kp1 = std::make_shared<Variable>(_mesh, "u_kp1");
    auto u_ref = std::make_shared<Variable>(_mesh, "u_ref");
    const double epsilon = 1e-5;  //seuil de convergence

    if (!_mesh) {
        std::cerr << "Erreur : Maillage nul. " << std::endl;    //affiche une erreur dans le cas où le maillage est null
        return;
    }
    
    //Jacobi résolution
    _equation.compute_exact_solution(*u_ref, _mesh.get());
    _equation.compute_initial_condition(*u_k, _mesh.get(), function); //utilisation de _mesh.get() pour obtenir un pointeur brut ver l'objet IMesh (avec utilisation de std::shared_ptr)
    _equation.compute_boundary_conditions(*u_kp1);

    std::chrono::time_point<std::chrono::system_clock> startJ, endJ;
    startJ = std::chrono::system_clock::now();
    std::cout << "--- Solving Problem ---" << std::endl;    //affichage du début de résolution du problème
    for (int i = 0; i < NombreIterations; i++) {
        if (has_converged(u_k, u_kp1, epsilon)) {
            std::cout << "Convergence atteinte pour iteration " << i << std::endl;  //renvoie l'itération pour laquelle on atteint la convergence
            break;
        }

        //std::cout << "--- Iterative method iteration : " << i + 1 << " ---" << std::endl;
        _equation.compute_for_solver<Jacobi>(u_k, u_kp1, _mesh.get());     //appel de Equation::compute via l'instance equation, permet de lancer le calcul
        _equation.compute_boundary_conditions(*u_kp1);
        std::swap(u_k, u_kp1);

        if (i == NombreIterations - 1) {std::cout << "Reached maximum iterations without convergence ---" << std::endl;}
    }
    endJ = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_secondsJ = (endJ - startJ)*1000;
    std::cout << "Elapsed time Jacobi solve: " << elapsed_secondsJ.count() << "ms\n";    
    
    auto u_k_gs = std::make_shared<Variable>(_mesh, "u_k_gs");
    auto u_kp1_gs = std::make_shared<Variable>(_mesh, "u_kp1_gs");
    auto u_ref_gs = std::make_shared<Variable>(_mesh, "u_ref_gs");
    //Gauss Seidel résolution 
    _equation.compute_exact_solution(*u_ref_gs, _mesh.get());
    _equation.compute_initial_condition(*u_k_gs, _mesh.get(), function); //utilisation de _mesh.get() pour obtenir un pointeur brut vers l'objet IMesh (avec utilisation de std::shared_ptr)
    _equation.compute_boundary_conditions(*u_kp1_gs);
    
    std::chrono::time_point<std::chrono::system_clock> startG, endG;
    startG = std::chrono::system_clock::now();
    std::cout << "--- Solving Problem ---" << std::endl;    //affichage du début de résolution du problème
    for (int i = 0; i < NombreIterations; i++) {
        if (has_converged(u_k_gs, u_kp1_gs, epsilon)) {
            std::cout << "Convergence atteinte pour iteration " << i << std::endl;  //renvoie l'itération pour laquelle on atteint la convergence
            break;
        }

        //std::cout << "--- Iterative method iteration : " << i + 1 << " ---" << std::endl;
        _equation.compute_for_solver<GaussSeidel>(u_k_gs, u_kp1_gs, _mesh.get());    //appel de Equation::compute via l'instance equation, permet de lancer le calcul
        _equation.compute_boundary_conditions(*u_kp1_gs);
        std::swap(u_k_gs, u_kp1_gs);

        if (i == NombreIterations - 1) {std::cout << "Reached maximum iterations without convergence ---" << std::endl;}
    }
    endG = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_secondsG = (endG - startG)*1000;
    std::cout << "Elapsed time Gauss Seidel solve: " << elapsed_secondsG.count() << "ms\n";

    double rapport = (elapsed_secondsG / elapsed_secondsJ );
    printf("Rapport de temps entre les deux méthodes : %f\n", rapport);

    double total_time = (elapsed_secondsG.count() + elapsed_secondsJ.count());
    printf("Temps total Gauss + Jacobi : %fms\n", total_time);

    //_equation.export_to_vtk(u_kp1, u_ref, u_k, _mesh.get());

}

/*Définit quand est ce qu'une solution itérative a convergé, on compare la solution actuelle à la solution précedente, 
on vérifie ensuite que la différence maximale entre les valeurs de u_kp1 et u_k est inférieure à une valeur de convergence 
epsilon fixée dans ce TP a 10e-6. Cette fonction renvoie true quand on a une convergence (quand la diff maximale est inférieure à epsilon),
et faux quand on a pas atteint la convergence*/
bool Problem::has_converged(std::shared_ptr<Variable> u_k, std::shared_ptr<Variable> u_kp1, double epsilon) {
    double max_diff = 0.0;
    
    // Calculer la norme infinie de la différence
    for (int i = 0; i < _mesh->nb_points(); ++i) {
        double diff = std::abs((*u_kp1)[i] - (*u_k)[i]);
        max_diff = std::max(max_diff, diff);
    }
    //std::cout << "Max difference: " << max_diff << std::endl;
    // Vérifier si la norme infinie est inférieure à epsilon
    return max_diff < epsilon;
}