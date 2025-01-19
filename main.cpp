#include "Problem.h"
#include "UniformMesh.h"

enum class MeshType { Uniform, NonUniform };

/*Permet de renvoyer vers un type de mesh*/
MeshType getMeshTypeFromString(const std::string& type) {
    if (type == "uniform") return MeshType::Uniform;
    if (type == "nonuniform") return MeshType::NonUniform; // NonUniform est une classe vide
    throw std::invalid_argument("Invalid mesh type : "+type);
}

int main(int argc, char** argv) {
    double x_min = 0.0, x_max = 1.0, dx = 0.01; //on initialise des paramètres par défaut 
    std::string mesh_type_str = "uniform";  //on initialise un mesh par défaut
    
    /*La suite permet de déterminer les informations rentrées par l'utilisateur et d'agir en fonction*/
    if (argc >= 5) {
        try {
            x_min = std::stod(argv[1]); //argument 1 correspond à x_min
            x_max = std::stod(argv[2]); //argument 2 correspond à x_max
            dx = std::stod(argv[3]);    //argument 3 correspond à dx (pas d'espace)
            mesh_type_str = argv[4];    //argument 4 correspond au type de mesh (uniform ou non uniform)

            if (x_min >= x_max || dx <= 0) {
                throw std::invalid_argument("Invalid mesh parameters.");    //contrôle que l'argument saisi par l'utilisateur est ok (xmin inférieur à xmax et dx positif)
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing arguments: " << e.what() << ". Using default parameters.\n";    //utilisation des paramètres par défaut en cas d'exception levée dans le try (erreur de saisie par ex)
        }

    } else {
        std::cerr << "Warning: Insufficient arguments. Using default parameters.\nTry next time : ./simulator 0.0 1.0 0.01 uniform|nonuniform\n";   //pas assez d'arguments, utilisation de paramètres par défaut
    }

    MeshType mesh_type = MeshType::Uniform;
    try { 
        mesh_type = getMeshTypeFromString(mesh_type_str);   //renvoie le meshtype correspondant à ce que saisie l'utilisateur!
    } catch (const std::exception& e) {
        std::cerr << "Invalid mesh type: " << e.what() << ". Defaulting to Uniform.\n"; //erreur si le mesh saisi est invalide ou non reconnu avec exception
    }
    
    std::shared_ptr<IMesh> mesh;
    try {
        if (mesh_type == MeshType::Uniform) {
            mesh = std::make_shared<UniformMesh>(x_min, x_max, dx); //renvoie à la méthode de mesh uniform
        } else {
            std::cerr << "NonUniform mesh type not yet implemented. Defaulting to Uniform.\n";  //nonuniform est une classe vide
            mesh = std::make_shared<UniformMesh>(x_min, x_max, dx); //renvoie à la méthode de mesh uniform
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating mesh: " << e.what() << ". Falling back to default Uniform mesh.\n";    //permet de relever les exceptions par ex lorsque qu'une classe est corrompu, cela permet de retourner au mesh par défaut
        mesh = std::make_shared<UniformMesh>(0.0, 1.0, 0.01);
    }

    if (!mesh) {
        std::cerr << "Failed to create a valid mesh.\n";    //vérifie que le mesh existe
        return 1;
    }

    try {
        Problem problem(mesh, 3000);
        problem.solve();
    } catch (const std::exception& e) {
        std::cerr << "Error solving problem: " << e.what() << '\n'; //renvoie une erreur si problem.solve() échoue
        return 1;
    }

    return 0;
}