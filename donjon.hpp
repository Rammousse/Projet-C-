#ifndef DONJON_HPP
#define DONJON_HPP

#include <vector>
#include <utility>
#include <fstream>
#include "case.hpp"

/** 
 * Codes ANSI pour la colorisation de la console, permettent de rendre l'affichage du donjon plus lisible
 */

// Codes ANSI pour les couleurs
const std::string RESET = "\033[0m";
const std::string ROUGE = "\033[31m";
const std::string VERT  = "\033[32m";
const std::string JAUNE = "\033[33m";
const std::string BLEU  = "\033[34m";
const std::string CYAN  = "\033[36m";
const std::string MAGENTA = "\033[35m";

/** 
 * Grille2D
 * Alias pour une matrice 2D de pointeurs vers des objets Case
 */

using Grille2D = std::vector<std::vector<Case*>>;

/**
 * Donjon
 * Classe responsable de la structure globale du labyrinthe.
 * 
 * Eelle gère l'allocation de la mémoire pour les cases, la génération procédurale, l'affichage graphique et la persistance des données (sauvegarde/chargement)
 */

class Donjon {
private:
    Grille2D grille; //matrice de pointeurs vers des objets Case (Polymorphisme)
    int largeur, hauteur;

    /**
     * Algorithme récursif de génération du labyrinthe avec coordonnées X et Y de départ
     */

    void genererLabyrinthe(int x, int y);

public:
    Donjon();
    ~Donjon(); // pour libérer la mémoire des pointeurs Case*
    
    /**
     * Initialise et génère un labyrinthe de dimensions données
     */
    void generer(int l, int h);

    /**
     * Affiche l'état actuel du donjon dans le terminal, avec une liste de coordonnées optionnelle pour afficher un chemin spécifique
     */
    void afficher(int playerX, int playerY, const std::vector<std::pair<int, int>>& chemin = {});

    /**
     * Place aléatoirement ou selon des règles des éléments (objets, monstres) dans le donjon
     */
    void placerElements();

    /**
     * Change le type d'une case aux coordonnées spécifiées, avec type : nouveau type de case à appliquer
     */
    void setCase(int x, int y, TypeCase type);

    /**
     *Sérialise l'état du donjon dans un fichier de sortie, avec ofs Flux de fichier de sortie ouvert
     */
    void sauvegarder(std::ofstream& ofs) const;

    /**
     * Désérialise et reconstruit le donjon depuis un fichier, avec ifs Flux de fichier d'entrée ouvert
     */
    void charger(std::ifstream& ifs);

    /**
     * Formate une ligne spécifique du donjon en chaîne de caractères pour l'affichage
     */
    std::string getLigneAffichage(int y, int playerX, int playerY, const std::vector<std::pair<int, int>>& chemin = {}); // La ligne formatée avec les éventuelles couleurs ANSI

    /**
     * Calcule le chemin le plus court vers une destination (généralement la sortie)
     * Utilise l'algorithme BFS 
     * retourne un vecteur de paires (x, y) représentant le chemin
     */
    std::vector<std::pair<int, int>> trouverChemin(int startX, int startY); // BFS
    
    // Getters utiles pour l'aventurier

    Case* getCase(int x, int y); // Récupère le pointeur vers la case aux coordonnées (x, y)
    int getLargeur() const { return largeur; } // Retourne la largeur du donjon
    int getHauteur() const { return hauteur; } // Retourne la hauteur du donjon
};

#endif