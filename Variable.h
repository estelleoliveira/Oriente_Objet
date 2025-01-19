#ifndef VARIABLE_H
#define VARIABLE_H
#include "IMesh.h"
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>

class Variable {
public : 
    Variable(std::shared_ptr<IMesh> mesh, const std::string& name = "") : _mesh(std::move(mesh)), _values(_mesh->nb_points(), 0.0), m_name(name) {}

    double& operator[](int i) {
        return _values[i];}

    const double& operator[](int i) const {
        return _values[i];}

    int size() const {
        return _mesh->nb_points();}
    
    auto begin() {
        return _mesh->x_min();}
    
    auto end() {
        return _mesh->x_max();}

    void print() const {
        std::ofstream file("Var_" + m_name + ".data");

        for (size_t i = 0; i < _values.size(); ++i) {
            file << i << " " << _values[i] << "\n";
        }
        file.close();

        //std::cout << "Variable: " << m_name << "\n";
        //for (const auto& v : _values) {
        //    std::cout << v << " ";
        //}
        //std::cout << "\n";
    }

private : 
    std::shared_ptr<IMesh> _mesh;
    std::vector<double> _values;
    std::string m_name;
};

#endif //VARIABLE_H