#include "aventurier.hpp"
#include <iostream>
#include <limits>

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
            resoudreCase(caseCible,d);
        } else {
            std::cout << "Action impossible : Il y a un mur !" << std::endl;
        }
    } else {
        std::cout << "Action impossible : Bord du donjon !" << std::endl;
    }
}


void Aventurier::resoudreCase(Case* c, Donjon& d) {
    char type = c->afficher();
    bool interaction = false; // Pour savoir si on doit faire une pause

    if (type == 'T') {
        std::cout << JAUNE << "\nTRESOR ! Vous ramassez une relique dorée." << RESET << std::endl;
        inventaire++;
        d.setCase(posX, posY, TypeCase::PASSAGE);
        interaction = true;
    } 
    else if (type == 'P') {
        std::cout << CYAN << "\nPIEGE ! -10 PV." << RESET << std::endl;
        pv -= 10;
        interaction = true;
    } 
    else if (type == 'M') {
        interaction = true;
        char choix;
        std::cout << ROUGE << "\n👾 UN MONSTRE S'APPROCHE ! " << RESET << std::endl;
        std::cout << "Voulez-vous (C)ombattre ou (F)uir ? ";
        std::cin >> choix;

        if (choix == 'c' || choix == 'C') {
            int chance = rand() % 100;
            if (chance > 40) {
                std::cout << VERT << "Victoire ! Vous avez terrassé le monstre." << RESET << std::endl;
                d.setCase(posX, posY, TypeCase::PASSAGE);
            } else {
                std::cout << ROUGE << "Le monstre est coriace... Vous perdez 30 PV." << RESET << std::endl;
                pv -= 30;
            }
        } else {
            std::cout << BLEU << "Vous fuyez courageusement (-5 PV)." << RESET << std::endl;
            pv -= 5;
        }
        // On vide le tampon après le choix du combat
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } 
    else if (type == 'S') {
        std::cout << VERT << "\n✨ LA SORTIE ! Vous avez réussi à vous échapper !" << RESET << std::endl;
        aGagne = true;
        interaction = true;
    }

    // Si on a eu un événement, on attend vraiment l'utilisateur
    if (interaction) {
        std::cout << "\nAppuyez sur [Entree] pour continuer...";
        // On s'assure que le tampon est vide avant d'attendre
        // std::cin.ignore est déjà appelé dans boucleDeJeu normalement
        std::cin.get(); 
    }
}

void Aventurier::boucleDeJeu(Donjon& d) {
    char touche;
    while (estVivant() && !aGagne) {
        // On calcule le chemin à chaque tour
        std::vector<std::pair<int, int>> cheminOpti = d.trouverChemin();
        
        // On l'envoie à l'affichage
        d.afficher(posX, posY, cheminOpti);
        
        // On affiche les stats en dessous
        afficherStatut();
        
        // On demande l'action
        std::cout << "\nActions : (Z)Haut (S)Bas (Q)Gauche (D)Droite | Quitter (X)" << std::endl;
        std::cout << "Votre choix >> ";
        std::cin >> touche;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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