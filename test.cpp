#include <gtest/gtest.h>
#include "IMesh.h"
#include "UniformMesh.h"
#include "Problem.h"
#include "Equation.h"

//Vérifie que le maillage est crée correctement
TEST(UnitaryTest, MeshTest) {
    UniformMesh mesh(0.0, 1.0, 0.2);
    EXPECT_EQ(mesh.x_min(), 0.0);
    EXPECT_EQ(mesh.x_max(), 1.0);
    EXPECT_EQ(mesh.dx(), 0.2);
}

//Test sur le maillage nul, on teste d'abord normalement avec un maillage classique, puis avec un maillage nul,
//on s'attend à avoir pas de soucis avec le maillage de base et un problème avec le programme qui renvoie maillage ne peut pas etre nul pour le maillage nul
TEST(UnitaryTest, ProblemTest) {
    auto mesh = std::make_unique<UniformMesh>(0.0, 1.0, 0.2);
    Problem _problem(std::move(mesh), 10);

    EXPECT_NO_THROW(_problem.solve());

    try {
        Problem _problemNull(nullptr, 10);
    } catch (const std::invalid_argument& e) {
        EXPECT_STREQ(e.what(), "Le pointeur de maillage ne peut pas être nul");
    }
}

//on vérifie qu'en donnant des variables, un maillage, on peut compute dans equation
TEST(MainFunctionTest, EquationTest) {
    auto mesh = std::make_shared<UniformMesh>(0.0, 1.0, 0.2);
    Equation _equation(mesh, 10);

    std::shared_ptr<Variable> u_k = std::make_shared<Variable>(mesh, "u_k");   //créer la variable u_k
    std::shared_ptr<Variable> u_kp1 = std::make_shared<Variable>(mesh, "u_kp1"); //créer la variable u_kp1
    
    EXPECT_NO_THROW(_equation.compute(u_k, u_kp1, mesh.get())); 
}
