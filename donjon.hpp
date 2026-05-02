#ifndef DONJON_HPP
#define DONJON_HPP

#include <vector>
#include <utility>
#include "case.hpp"

// Codes ANSI pour les couleurs
const std::string RESET = "\033[0m";
const std::string ROUGE = "\033[31m";
const std::string VERT  = "\033[32m";
const std::string JAUNE = "\033[33m";
const std::string BLEU  = "\033[34m";
const std::string CYAN  = "\033[36m";

using Grille2D = std::vector<std::vector<Case*>>;

class Donjon {
private:
    Grille2D grille;
    int largeur, hauteur;

    void genererLabyrinthe(int x, int y);

public:
    Donjon();
    ~Donjon(); // Pour libérer la mémoire des pointeurs Case*
    
    void generer(int l, int h);
    void afficher(int playerX, int playerY, const std::vector<std::pair<int, int>>& chemin = {});
    void placerElements();
    void setCase(int x, int y, TypeCase type);

    std::vector<std::pair<int, int>> trouverChemin(int startX, int startY); // BFS
    
    // Getters utiles pour l'aventurier
    Case* getCase(int x, int y);
    int getLargeur() const { return largeur; }
    int getHauteur() const { return hauteur; }
};

#endif