#include <iostream>
#include <ctime>
#include "donjon.hpp"
#include "aventurier.hpp"

int main() {
    // Initialisation de la graine aléatoire (crucial pour avoir un laby différent à chaque fois)
    srand(static_cast<unsigned>(time(nullptr)));

    // Création du Donjon
    // Note : Pour le Backtracking, il vaut mieux utiliser des nombres impairs (ex: 21x11)
    // pour que les murs et les chemins tombent bien.
    Donjon monDonjon;
    Aventurier joueur(1, 1);

    std::cout << "============================" << std::endl;
    std::cout << "   BIENVENUE DANS LE DONJON " << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "Objectif : Trouver la sortie et survivre !" << std::endl;
    std::cout << " " << std::endl;
    std::cout << "MODE DE JEU :" << std::endl;
    std::cout << "  1. Nouvelle partie" << std::endl;
    std::cout << "  2. Charger partie" << std::endl;
    std::cout << "Choix : ";

    int choix;
    std::cin >> choix;
    
    if (choix == 2) {
        std::ifstream ifs("sauvegarde.txt");
        if (ifs.is_open()) {
            joueur.charger(ifs);     // Charge d'abord les stats du joueur
            monDonjon.charger(ifs);  // Charge ensuite le donjon
            ifs.close();
            std::cout << "Partie chargée !" << std::endl;
        } else {
            std::cout << "Aucune sauvegarde trouvée. Lancement d'une nouvelle partie..." << std::endl;
            monDonjon.generer(21, 11);
        }
    } else {
        std::cout << "Génération du labyrinthe en cours..." << std::endl;
        monDonjon.generer(21, 11);
    }
    
    // Lancement de la boucle de jeu
    joueur.boucleDeJeu(monDonjon);

    return 0;
}