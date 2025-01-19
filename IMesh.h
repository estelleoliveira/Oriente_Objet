#ifndef IMESH_H
#define IMESH_H

class IMesh {
public : 
    virtual ~IMesh() = default;     //destructeur
    virtual double x_min() const = 0;   //retourne la position initiale 
    virtual double x_max() const = 0;   //retourne la position finale
    virtual double dx() const = 0;  //retourne le pas d'espace
    virtual int nb_points() const = 0;  //retourne le nombre de points du maillage
    virtual double x_i(int i) const = 0;    //retourne la position du point correspondant
};

#endif