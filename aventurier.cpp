#include "aventurier.hpp"
#include <iostream>

Aventurier::Aventurier(int startX, int startY) 
    : posX(startX), posY(startY), pv(100), inventaire(0) {}

bool Aventurier::estVivant() const {
    return pv > 0;
}

void Aventurier::afficherStatut() const {
    std::cout << "PV: " << pv << " | Tresors: " << inventaire 
              << " | Position: (" << posX << "," << posY << ")" << std::endl;
}

void Aventurier::deplacer(int dx, int dy, Donjon& d) {
    int nextX = posX + dx;
    int nextY = posY + dy;

    // Vérifier les limites du donjon et si la case n'est pas un MUR
    Case* c = d.getCase(nextX, nextY);
    if (c != nullptr && c->afficher() != '#') {
        posX = nextX;
        posY = nextY;
        resoudreCase(c); // On interagit avec la case
    } else {
        std::cout << "C'est un mur !" << std::endl;
    }
}

void Aventurier::resoudreCase(Case* c) {
    char type = c->afficher();
    if (type == 'T') {
        std::cout << "Vous avez trouve un Tresor !" << std::endl;
        inventaire++;
    } else if (type == 'M') {
        std::cout << "Un Monstre vous attaque ! -20 PV" << std::endl;
        pv -= 20;
    } else if (type == 'P') {
        std::cout << "CLAC ! Un piege ! -10 PV" << std::endl;
        pv -= 10;
    }
}

void Aventurier::boucleDeJeu(Donjon& d) {
    char touche;
    while (estVivant()) {
        d.afficher(); // On affiche le labyrinthe (l'étudiant 1 doit coder cette méthode)
        afficherStatut();
        std::cout << "Deplacement (z:haut, s:bas, q:gauche, d:droite) : ";
        std::cin >> touche;

        switch (touche) {
            case 'z': deplacer(0, -1, d); break;
            case 's': deplacer(0, 1, d);  break;
            case 'q': deplacer(-1, 0, d); break;
            case 'd': deplacer(1, 0, d);  break;
            default: std::cout << "Touche invalide." << std::endl; break;
        }
        
        // Optionnel : système pour effacer l'écran sous Linux
        // system("clear"); 
    }
    std::cout << "GAME OVER..." << std::endl;
}