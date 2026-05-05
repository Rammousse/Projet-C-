#ifndef JEUDUO_HPP
#define JEUDUO_HPP

#include "donjon.hpp"
#include "aventurier.hpp"
#include <utility>

/**
 * Classe qui gere le mode 2 joueurs (coop)
 * possede les 2 donjons et fait le lien entre eux
 */
class JeuDuo {
private:
    Donjon donjonA;
    Donjon donjonB;
    Aventurier joueurA;
    Aventurier joueurB;
    bool joueurAActif; // flag pr savoir qui on joue en ce moment (true = A, false = B)
    
    // on stocke la pos des portes pr pouvoir les ouvrir a distance
    std::pair<int, int> posPorteA;
    std::pair<int, int> posPorteB;

public:
    /**
     * Setup de base du mode coop
     */
    JeuDuo();

    /**
     * Cree les 2 donjons et place les leviers/portes croises
     */
    void genererMondes();

    /**
     * Affiche les 2 grilles cote a cote (split screen) et le hud
     */
    void afficherSplitScreen(const std::vector<std::pair<int, int>>& cheminA, const std::vector<std::pair<int, int>>& cheminB);

    /**
     * La grosse boucle du jeu duo (gere les inputs, le switch et les chronos)
     */
    void boucleDeJeu();

    /**
     * Check si un joueur est sur un levier pour ouvrir la porte de l'autre joueur
     */
    void verifierMecanismes(); 
    
    /**
     * save et load de toute la game coop
     */
    void sauvegarder(std::ofstream& ofs) const;
    void charger(std::ifstream& ifs);

    // getters pr l'ecran de fin et le calcul du % d'optimisation
    int getDistanceOptimaleA() { return donjonA.trouverChemin(1,1).size() > 0 ? donjonA.trouverChemin(1,1).size() - 1 : 0; }
    int getDistanceOptimaleB() { return donjonB.trouverChemin(1,1).size() > 0 ? donjonB.trouverChemin(1,1).size() - 1 : 0; }
    Aventurier& getJoueurA() { return joueurA; }
    Aventurier& getJoueurB() { return joueurB; }
};

#endif