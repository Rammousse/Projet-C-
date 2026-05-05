#include "aventurier.hpp"
#include <iostream>
#include <limits>
#include <random>
#include <chrono>

/**
 * Setup de base du joueur
 */
Aventurier::Aventurier(int startX, int startY) 
    : posX(startX), posY(startY), pv(100), inventaire(0), aGagne(false),
        pasEffectues(0), combatsGagnes(0), piegesSubis(0), tempsCumule(0) 
{
    // on oublie pas de compter la tte 1ere case !
    casesVisitees.insert({startX, startY}); 
}

/**
 * Check si le joueur est tjs en vie
 */
bool Aventurier::estVivant() const {
    return pv > 0;
}

/**
 * Affiche la barre de vie en ascii et l'inventaire
 */
void Aventurier::afficherStatut() const {
    std::cout << "\n--- STATS ---" << std::endl;
    
    // met en rouge si on est low hp, sinon vert
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

/**
 * Gere les deplacements sur la grille et les collisions
 */
void Aventurier::deplacer(int dx, int dy, Donjon& d) {
    int cibleX = posX + dx;
    int cibleY = posY + dy;
    Case* caseCible = d.getCase(cibleX, cibleY);

    if (caseCible != nullptr) {
        // on bloque sur les murs et les portes fermees
        if (caseCible->afficher() != '#' && caseCible->afficher() != 'D') {
            posX = cibleX;
            posY = cibleY;
            
            pasEffectues++;
            casesVisitees.insert({posX, posY});
            
            resoudreCase(caseCible, d);
        } else {
            // cas ou on se prend un mur ou une porte
            if (caseCible->afficher() == 'D') {
                std::cout << MAGENTA << "\nLa porte est verrouillée ! Votre partenaire doit trouver un levier." << RESET << std::endl;
            } else {
                std::cout << ROUGE << "\nAction impossible : Il y a un mur !" << RESET << std::endl;
            }
            std::cout << "Appuyez sur [Entree] pour continuer...";
            std::cin.get(); 
        }
    } else {
        std::cout << ROUGE << "\nAction impossible : Bord du donjon !" << RESET << std::endl;
        std::cout << "Appuyez sur [Entree] pour continuer...";
        std::cin.get(); 
    }
}

/**
 * Applique l'effet de la case ou le joueur vient d'arriver
 */
void Aventurier::resoudreCase(Case* c, Donjon& d) {
    char type = c->afficher();
    bool interaction = false; // flag pr savoir si on fait une pause clavier

    if (type == 'T') {
        std::cout << JAUNE << "\nTRESOR ! Vous ramassez une relique dorée." << RESET << std::endl;
        inventaire++;
        d.setCase(posX, posY, TypeCase::PASSAGE);
        interaction = true;
    } 
    else if (type == 'P') {
        std::cout << CYAN << "\nPIEGE ! -10 PV." << RESET << std::endl;
        pv -= 10;
        piegesSubis++;
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
                engagerCombat(monstre, d); 
            }
        } else {
            std::cout << BLEU << "Vous fuyez courageusement (-5 PV)." << RESET << std::endl;
            this->recevoirDegats(5); // evite les hp negatifs
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
    }
    else if (type == 'S') {
        std::cout << VERT << "\n✨ LA SORTIE ! Vous avez réussi à vous échapper !" << RESET << std::endl;
        aGagne = true;
        interaction = true;
    }

    // pause si un truc s'est passé
    if (interaction) {
        std::cout << "\nAppuyez sur [Entree] pour continuer...";
        std::cin.get(); 
    }
}

/**
 * Grosse boucle principale de la game
 */
void Aventurier::boucleDeJeu(Donjon& d) {
    char touche;
    bool afficherChemin = false; // mémorise si on affiche l'aide opti

    // start chrono
    auto startTimer = std::chrono::steady_clock::now();

    while (estVivant() && !aGagne) {

        std::vector<std::pair<int, int>> cheminOpti;
        
        // calcul bfs si demandé
        if (afficherChemin) {
            cheminOpti = d.trouverChemin(posX, posY);
        }
        
        // On l'envoie à l'affichage
        d.afficher(posX, posY, cheminOpti);
        afficherStatut();
        
        // pour demander l'action
        std::cout << "\nActions : (Z)Haut (S)Bas (Q)Gauche (D)Droite | (P)Chemin | (V)Sauvegarder | Quitter (X)" << std::endl;
        std::cout << "Votre choix >> ";
        std::cin >> touche;

        // nettoie le buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (touche == 'x' || touche == 'X') break;

        if (touche == 'p' || touche == 'P') {
            afficherChemin = !afficherChemin; // toggle l'affichage
            continue; 
        }

        if (touche == 'v' || touche == 'V') {
            auto now = std::chrono::steady_clock::now();
            tempsCumule += std::chrono::duration_cast<std::chrono::seconds>(now - startTimer).count();
            startTimer = std::chrono::steady_clock::now();

            std::ofstream ofs("sauvegarde.txt");
            if (ofs.is_open()) {
                ofs << "1\n";           // flag mode solo pr la save
                this->sauvegarder(ofs);
                d.sauvegarder(ofs);
                std::cout << VERT << "Partie sauvegardée avec succès !" << RESET << std::endl;
                ofs.close();
            } else {
                std::cout << ROUGE << "Erreur lors de la sauvegarde." << RESET << std::endl;
            }
            std::cin.get(); 
            continue;
        }

        // inputs deplacements
        if (touche == 'z') deplacer(0, -1, d);
        else if (touche == 's') deplacer(0, 1, d);
        else if (touche == 'q') deplacer(-1, 0, d);
        else if (touche == 'd') deplacer(1, 0, d);
    }

    // fin de run on save le temps
    auto endTimer = std::chrono::steady_clock::now();
    tempsCumule += std::chrono::duration_cast<std::chrono::seconds>(endTimer - startTimer).count();
}

/**
 * Conserve les stats et pos ds le fichier txt
 */
void Aventurier::sauvegarder(std::ofstream& ofs) const {
    ofs << posX << " " << posY << " " << pv << " " << inventaire << " " << aGagne << "\n";
    
    // save des stats
    ofs << pasEffectues << " " << combatsGagnes << " " << piegesSubis << " " << tempsCumule << " " << casesVisitees.size() << "\n";
    
    for (auto const& coord : casesVisitees) {
        ofs << coord.first << " " << coord.second << " ";
    }
    ofs << "\n";
}

/**
 * Recharge la game depuis le fichier txt
 */
void Aventurier::charger(std::ifstream& ifs) {
    ifs >> posX >> posY >> pv >> inventaire >> aGagne;
    
    int nbCases;
    ifs >> pasEffectues >> combatsGagnes >> piegesSubis >> tempsCumule >> nbCases;
    
    casesVisitees.clear();
    for (int i = 0; i < nbCases; ++i) {
        int cx, cy;
        ifs >> cx >> cy;
        casesVisitees.insert({cx, cy});
    }
}

/**
 * Petit utilitaire pr dessiner la barre de pv ds l'ecran de combat
 */
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

/**
 * Gere l'interface et les jets de des du combat contre un mob
 */
void Aventurier::engagerCombat(Monstre* m, Donjon& d) {
    // setup du rng
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(1, 100);

    bool fuite = false;

    while (this->estVivant() && m->estVivant() && !fuite) {
        system("clear");
        
        // beau pti dessin
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
        
        afficherBarreVie(this->pv, 100, "VOUS   ");
        afficherBarreVie(m->getPv(), 50, "MONSTRE");
        
        std::cout << "\nChoisissez votre attaque :" << std::endl;
        std::cout << "1. Légère (Dégâts: 15 | Précision: 90%)" << std::endl;
        std::cout << "2. Moyenne (Dégâts: 25 | Précision: 60%)" << std::endl;
        std::cout << "3. Lourde  (Dégâts: 40 | Précision: 30%)" << std::endl;
        std::cout << "4. Fuir (-10 PV)" << std::endl;
        std::cout << ">> ";
        
        char choix;
        std::cin >> choix;
        // flush du buffer
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
            std::cin.get(); 
            break; 
        }
        else continue; // missclick on recommence le tour

        // recap de l'attaque
        std::cout << "\nChance de toucher : [";
        for(int i=10; i<=100; i+=10) {
            if(i <= proba) std::cout << VERT << "=" << RESET;
            else std::cout << ROUGE << "-" << RESET;
        }
        std::cout << "] " << proba << "%" << std::endl;
        
        std::cout << "Confirmer l'attaque ? (o/n) : ";
        char conf;
        std::cin >> conf;
        
        // re flush
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (conf != 'o' && conf != 'O') continue; // annule

        // resolution du hit
        int jet = dis(gen);
        std::cout << "\n>>> ";
        if (jet <= proba) {
            std::cout << VERT << "BOOM ! Vous touchez le monstre !" << RESET << std::endl;
            m->recevoirDegats(degats);
            std::cout << "Le monstre perd " << degats << " PV." << std::endl;
        } else {
            std::cout << ROUGE << "SWOOSH ! Vous ratez votre cible..." << RESET << std::endl;
        }

        // tour du mob s'il est pas crevé
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

        std::cout << "\nAppuyez sur [Entree] pour continuer...";
        std::cin.get(); 

        // supprime le mob du plateau si on win
        if (!m->estVivant()) {
            d.setCase(posX, posY, TypeCase::PASSAGE);
            combatsGagnes++;
            break; 
        }
    }
}

/**
 * Affiche l'ecran de stat a la fin de la game (win ou loose)
 */
void Aventurier::afficherRapportFinal(int cheminMinimal) const {
    system("clear");
    std::string couleurTitre = aGagne ? "\033[33m" : "\033[31m"; 
    std::string etat = aGagne ? "VICTOIRE !" : "DÉCÈS...";

    std::cout << couleurTitre << R"(
  =========================================
            RAPPORT D'EXPLORATION         
  =========================================
)" << RESET;
    std::cout << "  Résultat    : " << couleurTitre << etat << RESET << "\n";
    std::cout << "  Temps passé : " << tempsCumule << " s\n\n";
    
    std::cout << BLEU << "  -- EXPLORATION --" << RESET << "\n";
    std::cout << "  Cases uniques visitées : " << casesVisitees.size() << "\n";
    std::cout << "  Distance parcourue     : " << pasEffectues << " pas\n";
    
    if (cheminMinimal > 0) {
        std::cout << "  Distance optimale BFS  : " << cheminMinimal << " pas\n";
        
        if (aGagne) {
            // calcul du vrai % d'opti
            int ratio = (pasEffectues > 0) ? (cheminMinimal * 100) / pasEffectues : 0;
            
            // max 100% mm si on est fort
            if (ratio > 100) ratio = 100; 
            
            std::cout << "  Efficacité             : " << ratio << "%\n\n";
        } else {
            std::cout << "  Efficacité             : N/A (Mort avant la sortie)\n\n";
        }
    }

    std::cout << ROUGE << "  -- COMBAT & SURVIE --" << RESET << "\n";
    std::cout << "  Monstres vaincus : " << combatsGagnes << "\n";
    std::cout << "  Pièges déclenchés: " << piegesSubis << "\n";
    std::cout << "  Trésors récoltés : " << JAUNE << inventaire << RESET << "\n";
    std::cout << "  =========================================\n\n";
}