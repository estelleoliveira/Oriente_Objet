#ifndef UNIFORMMESH_H
#define UNIFORMESH_H
#include "IMesh.h"


class UniformMesh : public IMesh {
public : 
    UniformMesh(double x_min, double x_max, double dx) : _x_min(x_min), _x_max(x_max), _dx(dx) {}
    int nb_points() const override {
        return static_cast<int>((_x_max - _x_min)/_dx) + 1; //calcul du nombre de points du maillage
    }

    
    double x_min() const override {return _x_min;}
    double x_max() const override {return _x_max;}
    double dx() const override {return _dx;}
    double x_i(int i) const override {return _x_min + i * _dx;}

private : 
    double _x_min;
    double _x_max; 
    double _dx; 
};

#endif