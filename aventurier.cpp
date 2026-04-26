#include "aventurier.hpp"
#include <iostream>

Aventurier::Aventurier(int startX, int startY) 
    : posX(startX), posY(startY), pv(100), inventaire(0), aGagne(false) {}

bool Aventurier::estVivant() const {
    return pv > 0;
}

void Aventurier::afficherStatut() const {
    std::cout << "\n--- STATS ---" << std::endl;
    
    // Si PV faibles, on affiche en rouge, sinon en vert
    std::string couleurPV = (pv > 30) ? "\033[32m" : "\033[31m";
    
    std::cout << "PV : " << couleurPV << pv << RESET << " / 100 [";
    int barres = pv / 10;
    for(int i=0; i<10; i++) {
        if(i < barres) std::cout << couleurPV << "=" << RESET;
        else std::cout << " ";
    }
    std::cout << "]" << std::endl;
    
    std::cout << "Trésors : " << JAUNE << inventaire << RESET << std::endl;
}

void Aventurier::deplacer(int dx, int dy, Donjon& d) {
    // 1. Calculer la position théorique
    int cibleX = posX + dx;
    int cibleY = posY + dy;

    // 2. Récupérer la case à cette position auprès du donjon
    Case* caseCible = d.getCase(cibleX, cibleY);

    // 3. Vérifier si le déplacement est possible
    if (caseCible != nullptr) {
        // Si la case n'est pas un mur (symbole '#' défini dans case.hpp)
        if (caseCible->afficher() != '#') {
            posX = cibleX;
            posY = cibleY;
            
            // Une fois déplacé, on résout l'effet de la case (Trésor, Monstre, etc.)
            resoudreCase(caseCible);
        } else {
            std::cout << "Action impossible : Il y a un mur !" << std::endl;
        }
    } else {
        std::cout << "Action impossible : Bord du donjon !" << std::endl;
    }
}

void Aventurier::resoudreCase(Case* c) {
    char type = c->afficher();
    if (type == 'S') {
        std::cout << "\033[32m" << "BRAVO ! Vous avez trouvé la sortie !" << "\033[0m" << std::endl;
        aGagne = true;
    }
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
    while (estVivant() && !aGagne) {
        // 1. On affiche la carte avec le joueur dessus
        d.afficher(posX, posY); 
        
        // 2. On affiche les stats en dessous
        afficherStatut();
        
        // 3. On demande l'action
        std::cout << "\nActions : (Z)Haut (S)Bas (Q)Gauche (D)Droite | Quitter (X)" << std::endl;
        std::cout << "Votre choix >> ";
        std::cin >> touche;

        if (touche == 'x' || touche == 'X') break;

        // Gestion des directions
        if (touche == 'z') deplacer(0, -1, d);
        else if (touche == 's') deplacer(0, 1, d);
        else if (touche == 'q') deplacer(-1, 0, d);
        else if (touche == 'd') deplacer(1, 0, d);
    }

    if (aGagne) {
        std::cout << "\033[33m" << "VICTOIRE ! Trésors récoltés : " << inventaire << "\033[0m" << std::endl;
    } else if (!estVivant()) {
        std::cout << "GAME OVER..." << std::endl;
    }
}