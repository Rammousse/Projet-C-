#ifndef JEUDUO_HPP
#define JEUDUO_HPP

#include "donjon.hpp"
#include "aventurier.hpp"
#include <utility>

class JeuDuo {
private:
    Donjon donjonA;
    Donjon donjonB;
    Aventurier joueurA;
    Aventurier joueurB;
    bool joueurAActif; // TRUE = on contrôle A, FALSE = on contrôle B
    
    std::pair<int, int> posPorteA;
    std::pair<int, int> posPorteB;

public:
    JeuDuo();
    void genererMondes();
    void afficherSplitScreen(const std::vector<std::pair<int, int>>& cheminA, const std::vector<std::pair<int, int>>& cheminB);
    void boucleDeJeu();
    void verifierMecanismes(); 
    
    void sauvegarder(std::ofstream& ofs) const;
    void charger(std::ifstream& ifs);

    // Accesseurs pour les stats de fin de partie
    int getDistanceOptimaleA() { return donjonA.trouverChemin(1,1).size() > 0 ? donjonA.trouverChemin(1,1).size() - 1 : 0; }
    int getDistanceOptimaleB() { return donjonB.trouverChemin(1,1).size() > 0 ? donjonB.trouverChemin(1,1).size() - 1 : 0; }
    Aventurier& getJoueurA() { return joueurA; }
    Aventurier& getJoueurB() { return joueurB; }
};

#endif