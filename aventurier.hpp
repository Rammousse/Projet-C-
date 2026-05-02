#ifndef AVENTURIER_HPP
#define AVENTURIER_HPP

#include <fstream>
#include <set>
#include <utility>
#include "donjon.hpp"

class Aventurier {
private:
    int posX, posY;
    int pv;
    int inventaire; // Nombre de trésors
    bool aGagne; // dit si le joueur a gagne

    int pasEffectues;
    int combatsGagnes;
    int piegesSubis;
    long long tempsCumule; // En secondes
    std::set<std::pair<int, int>> casesVisitees;

public:
    Aventurier(int startX, int startY);

    // Getters
    int getX() const { return posX; }
    int getY() const { return posY; }

    void deplacer(int dx, int dy, Donjon& d);
    void resoudreCase(Case* c, Donjon& d);
    bool estVivant() const;
    void afficherStatut() const;
    
    // La méthode principale qui gère les entrées clavier
    void boucleDeJeu(Donjon& d);

    void sauvegarder(std::ofstream& ofs) const;
    void charger(std::ifstream& ifs);

    void engagerCombat(Monstre* m, Donjon& d);
    void afficherBarreVie(int pvActuels, int pvMax, const std::string& nom) const;
    void recevoirDegats(int d) { pv -= d; if(pv < 0) pv = 0; }
    
    void afficherRapportFinal(int cheminMinimal) const;
};

#endif