#ifndef DONJON_HPP
#define DONJON_HPP

#include <vector>
#include <utility>
#include "case.hpp"

using Grille2D = std::vector<std::vector<Case*>>;

class Donjon {
private:
    Grille2D grille;
    int largeur, hauteur;

public:
    Donjon();
    ~Donjon(); // Pour libérer la mémoire des pointeurs Case*
    
    void generer(int l, int h);
    void afficher();
    std::vector<std::pair<int, int>> trouverChemin(); // BFS
    
    // Getters utiles pour l'aventurier
    Case* getCase(int x, int y);
    int getLargeur() const { return largeur; }
    int getHauteur() const { return hauteur; }
};

#endif