#include "aventurier.hpp"
#include <iostream>
#include <limits>
#include <random>
#include <chrono>

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
        char choix;
        std::cout << ROUGE << "\nUN MONSTRE S'APPROCHE ! " << RESET << std::endl;
        std::cout << "Voulez-vous (C)ombattre ou (F)uir ? ";
        std::cin >> choix;

        if (choix == 'c' || choix == 'C') {
            Monstre* monstre = dynamic_cast<Monstre*>(c);
            if (monstre != nullptr) {
                engagerCombat(monstre, d); // Lance l'interface visuelle
            }
        } else {
            std::cout << BLEU << "Vous fuyez courageusement (-5 PV)." << RESET << std::endl;
            this->recevoirDegats(5); // On utilise la nouvelle méthode pour éviter les PV négatifs
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
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
    bool afficherChemin = false; // on mémorise si on doit afficher le chemin
    while (estVivant() && !aGagne) {

        // On prépare un chemin vide par défaut
        std::vector<std::pair<int, int>> cheminOpti;
        
        // Si le joueur a demandé le chemin (touche 'p'), on le calcule
        if (afficherChemin) {
            cheminOpti = d.trouverChemin(posX, posY);
        }
        
        // On l'envoie à l'affichage
        d.afficher(posX, posY, cheminOpti);
        
        // On affiche les stats en dessous
        afficherStatut();
        
        // On demande l'action
        std::cout << "\nActions : (Z)Haut (S)Bas (Q)Gauche (D)Droite | (P)Chemin | (V)Sauvegarder | Quitter (X)" << std::endl;
        std::cout << "Votre choix >> ";
        std::cin >> touche;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (touche == 'x' || touche == 'X') break;

        if (touche == 'p' || touche == 'P') {
            afficherChemin = !afficherChemin; // Inverse l'état (true devient false, false devient true)
            continue; // On recharge l'affichage immédiatement
        }

        if (touche == 'v' || touche == 'V') {
            std::ofstream ofs("sauvegarde.txt");
            if (ofs.is_open()) {
                this->sauvegarder(ofs); // Sauvegarde l'aventurier
                d.sauvegarder(ofs);     // Sauvegarde le donjon
                std::cout << VERT << "Partie sauvegardée avec succès !" << RESET << std::endl;
                ofs.close();
            } else {
                std::cout << ROUGE << "Erreur lors de la sauvegarde." << RESET << std::endl;
            }
            std::cin.get(); // Pause pour lire le message
            continue;
        }

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

void Aventurier::sauvegarder(std::ofstream& ofs) const {
    ofs << posX << " " << posY << " " << pv << " " << inventaire << " " << aGagne << "\n";
}

void Aventurier::charger(std::ifstream& ifs) {
    ifs >> posX >> posY >> pv >> inventaire >> aGagne;
}

// Fonction utilitaire pour dessiner les barres de vie
void Aventurier::afficherBarreVie(int pvActuels, int pvMax, const std::string& nom) const {
    std::string couleur = (pvActuels > pvMax / 3) ? VERT : ROUGE;
    std::cout << nom << " : " << couleur << pvActuels << RESET << "/" << pvMax << " [";
    int barres = (pvActuels * 10) / pvMax;
    for(int i = 0; i < 10; i++) {
        if(i < barres) std::cout << couleur << "=" << RESET;
        else std::cout << " ";
    }
    std::cout << "]" << std::endl;
}

// Boucle principale du combat
void Aventurier::engagerCombat(Monstre* m, Donjon& d) {
    // Initialisation du générateur aléatoire
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(1, 100);

    bool fuite = false;

    while (this->estVivant() && m->estVivant() && !fuite) {
        system("clear");
        
        // 1. Affichage de l'ASCII art du Monstre
        std::cout << ROUGE << R"(
               /(.-""-.)\
           |\  \/      \/  /|
           | \ / =.  .= \ / |
           \( \   o\/o   / )/
            \_, '-/  \-' ,_/
              /   \__/   \
              \ \__/\__/ /
            ___\ \|--|/ /___
           /`    \    /    `\
        )" << RESET << "\n";
        
        std::cout << "======= COMBAT EN COURS =======" << std::endl;
        
        // 2. Affichage des barres de vie
        afficherBarreVie(this->pv, 100, "VOUS   ");
        afficherBarreVie(m->getPv(), 50, "MONSTRE");
        
        // 3. Choix de l'attaque
        std::cout << "\nChoisissez votre attaque :" << std::endl;
        std::cout << "1. Légère (Dégâts: 15 | Précision: 90%)" << std::endl;
        std::cout << "2. Moyenne (Dégâts: 25 | Précision: 60%)" << std::endl;
        std::cout << "3. Lourde  (Dégâts: 40 | Précision: 30%)" << std::endl;
        std::cout << "4. Fuir (-10 PV)" << std::endl;
        std::cout << ">> ";
        
        // ... (début de la fonction inchangé) ...
        
        char choix;
        std::cin >> choix;
        // 1. ON VIDE LE TAMPON TOUT DE SUITE APRÈS LA SAISIE
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        int proba = 0, degats = 0;
        if (choix == '1') { proba = 90; degats = 15; }
        else if (choix == '2') { proba = 60; degats = 25; }
        else if (choix == '3') { proba = 30; degats = 40; }
        else if (choix == '4') { 
            std::cout << BLEU << "Vous fuyez lâchement (-10 PV)..." << RESET << std::endl;
            this->recevoirDegats(10);
            fuite = true;
            
            std::cout << "\nAppuyez sur [Entree] pour fuir...";
            std::cin.get(); // Le tampon est déjà propre, on attend juste Entrée
            break; 
        }
        else continue; // Touche invalide, on recommence le tour

        // 4. Barre de probabilité visuelle et Confirmation
        std::cout << "\nChance de toucher : [";
        for(int i=10; i<=100; i+=10) {
            if(i <= proba) std::cout << VERT << "=" << RESET;
            else std::cout << ROUGE << "-" << RESET;
        }
        std::cout << "] " << proba << "%" << std::endl;
        
        std::cout << "Confirmer l'attaque ? (o/n) : ";
        char conf;
        std::cin >> conf;
        // 2. ON VIDE LE TAMPON ICI AUSSI
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (conf != 'o' && conf != 'O') continue; // Annulation de l'attaque

        // 5. Résolution de l'attaque du joueur
        int jet = dis(gen);
        std::cout << "\n>>> ";
        if (jet <= proba) {
            std::cout << VERT << "BOOM ! Vous touchez le monstre !" << RESET << std::endl;
            m->recevoirDegats(degats);
            std::cout << "Le monstre perd " << degats << " PV." << std::endl;
        } else {
            std::cout << ROUGE << "SWOOSH ! Vous ratez votre cible..." << RESET << std::endl;
        }

        // 6. Riposte du monstre (s'il n'est pas mort)
        if (m->estVivant()) {
            std::cout << "\n>>> " << ROUGE << "Le monstre riposte !" << RESET << std::endl;
            if (dis(gen) <= 70) { 
                int degatsM = m->getDegats();
                this->recevoirDegats(degatsM);
                std::cout << ROUGE << "Il vous frappe et inflige " << degatsM << " dégâts !" << RESET << std::endl;
            } else {
                std::cout << VERT << "Vous esquivez de justesse l'attaque du monstre !" << RESET << std::endl;
            }
        } else {
            std::cout << JAUNE << "\nVICTOIRE ! Le monstre s'effondre en poussière." << RESET << std::endl;
        }

        // 3. LA PAUSE DE FIN DE TOUR EST MAINTENANT SIMPLIFIÉE
        std::cout << "\nAppuyez sur [Entree] pour continuer...";
        std::cin.get(); // Fonctionnera parfaitement à chaque fois !

        if (!m->estVivant()) {
            d.setCase(posX, posY, TypeCase::PASSAGE); 
            break; 
        }
    }
}