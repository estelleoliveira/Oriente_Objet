#ifndef NONUNIFORMMESH_H
#define NONUNIFORMMESH_H
#include "IMesh.h"

class NonUniformMesh : public IMesh {
public:
    //implémentations vides pour les méthodes de IMesh
    int nb_points() const override {return 0;}  
    
    double x_i(int i) const override {return 0.0;}  

    double dx() const override {return 0.0;}
};
#endif //NONUNIFORMMESH_H