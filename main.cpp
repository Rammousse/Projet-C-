#include <iostream>
#include <ctime>
#include "donjon.hpp"
#include "aventurier.hpp"

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    Donjon monDonjon;
    monDonjon.generer(20, 10); // Exemple de taille

    Aventurier joueur(1, 1);
    
    std::cout << "DEBUT DU JEU" << std::endl;
    joueur.boucleDeJeu(monDonjon);

    return 0;
}