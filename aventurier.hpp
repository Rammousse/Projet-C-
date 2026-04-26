#ifndef AVENTURIER_HPP
#define AVENTURIER_HPP

#include "donjon.hpp"

class Aventurier {
private:
    int posX, posY;
    int pv;
    int inventaire; // Nombre de trésors

public:
    Aventurier(int startX, int startY);

    // Getters
    int getX() const { return posX; }
    int getY() const { return posY; }

    void deplacer(int dx, int dy, Donjon& d);
    void resoudreCase(Case* c);
    bool estVivant() const;
    void afficherStatut() const;
    
    // La méthode principale qui gère les entrées clavier
    void boucleDeJeu(Donjon& d);
};

#endif