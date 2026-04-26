#include <iostream>
#include <ctime>
#include "donjon.hpp"
#include "aventurier.hpp"

int main() {
    // 1. Initialisation de la graine aléatoire (crucial pour avoir un laby différent à chaque fois)
    srand(static_cast<unsigned>(time(nullptr)));

    // 2. Création du Donjon
    // Note : Pour le Backtracking, il vaut mieux utiliser des nombres impairs (ex: 21x11)
    // pour que les murs et les chemins tombent bien.
    Donjon monDonjon;
    int largeur = 21; 
    int hauteur = 11;

    std::cout << "Génération du labyrinthe en cours..." << std::endl;
    monDonjon.generer(largeur, hauteur); 

    // 3. Création de l'Aventurier
    // On le place en (1, 1) car l'algorithme commence souvent à creuser là.
    Aventurier joueur(1, 1);
    
    std::cout << "============================" << std::endl;
    std::cout << "   BIENVENUE DANS LE DONJON " << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "Commandes : z (haut), s (bas), q (gauche), d (droite)" << std::endl;
    std::cout << "Objectif : Trouver la sortie et survivre !" << std::endl;

    // 4. Lancement de la boucle de jeu
    joueur.boucleDeJeu(monDonjon);

    return 0;
}