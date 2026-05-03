#include "jeuduo.hpp"
#include <iostream>
#include <limits>
#include <chrono>

JeuDuo::JeuDuo() : joueurA(1, 1), joueurB(1, 1), joueurAActif(true) {}

void JeuDuo::genererMondes() {
    // 1. Génération des labyrinthes bruts et des pièges/monstres
    donjonA.generer(21, 11);
    donjonB.generer(21, 11);

    // 2. Récupération des chemins optimaux via BFS
    std::vector<std::pair<int, int>> cheminA = donjonA.trouverChemin(1, 1);
    std::vector<std::pair<int, int>> cheminB = donjonB.trouverChemin(1, 1);

    // Sécurité : on s'assure que les chemins sont assez longs pour y placer des éléments
    if (cheminA.size() > 5 && cheminB.size() > 5) {
        
        // --- PLACEMENT POUR A ---
        // Levier A (actionné par A) : placé à 25% de la progression de A
        int indexLevierA = cheminA.size() / 4;
        donjonA.setCase(cheminA[indexLevierA].first, cheminA[indexLevierA].second, TypeCase::LEVIER);
        
        // Porte A (bloque A) : placée à 75% de la progression de A
        int indexPorteA = cheminA.size() * 3 / 4;
        donjonA.setCase(cheminA[indexPorteA].first, cheminA[indexPorteA].second, TypeCase::PORTE);
        posPorteA = cheminA[indexPorteA]; // Mémorisation pour le levier B

        // --- PLACEMENT POUR B ---
        // Levier B (actionné par B) : placé à 25% de la progression de B
        int indexLevierB = cheminB.size() / 4;
        donjonB.setCase(cheminB[indexLevierB].first, cheminB[indexLevierB].second, TypeCase::LEVIER);
        
        // Porte B (bloque B) : placée à 75% de la progression de B
        int indexPorteB = cheminB.size() * 3 / 4;
        donjonB.setCase(cheminB[indexPorteB].first, cheminB[indexPorteB].second, TypeCase::PORTE);
        posPorteB = cheminB[indexPorteB]; // Mémorisation pour le levier A
    }
}

void JeuDuo::afficherSplitScreen(const std::vector<std::pair<int, int>>& cheminA, const std::vector<std::pair<int, int>>& cheminB) {
    system("clear");
    
    // Indicateurs visuels pour savoir qui on contrôle
    std::string curseurA = joueurAActif ? "\033[47m\033[30m[ EN CONTRÔLE ]\033[0m" : "               ";
    std::string curseurB = !joueurAActif ? "\033[47m\033[30m[ EN CONTRÔLE ]\033[0m" : "               ";

    std::cout << "\n    ==== AVENTURIER A ==== " << curseurA << "      ==== AVENTURIER B ==== " << curseurB << "\n\n";
    
    // Affichage des grilles
    for (int y = 0; y < donjonA.getHauteur(); ++y) {
        std::string ligneA = donjonA.getLigneAffichage(y, joueurA.getX(), joueurA.getY(), cheminA);
        std::string ligneB = donjonB.getLigneAffichage(y, joueurB.getX(), joueurB.getY(), cheminB);
        
        std::cout << ligneA << "   ||   " << ligneB << std::endl;
    }

    // --- INTERFACE GRAPHIQUE ---
    std::cout << "\n  PV : \033[32m" << joueurA.getPv() << "\033[0m/100" 
              << std::string(32, ' ') // Espacement
              << "PV : \033[32m" << joueurB.getPv() << "\033[0m/100" << std::endl;
              
    std::cout << "  Trésors : \033[33m" << joueurA.getInventaire() << "\033[0m" 
              << std::string(35, ' ') 
              << "Trésors : \033[33m" << joueurB.getInventaire() << "\033[0m" << std::endl;
}

void JeuDuo::verifierMecanismes() {
    // 1. On regarde la case sous le Joueur A
    Case* caseSousA = donjonA.getCase(joueurA.getX(), joueurA.getY());
    if (caseSousA && caseSousA->afficher() == 'L') {
        // Le joueur A est sur un Levier non activé !
        Levier* levier = dynamic_cast<Levier*>(caseSousA);
        if (levier) levier->activer();
        
        // On ouvre la porte du Joueur B !!
        Case* casePorteB = donjonB.getCase(posPorteB.first, posPorteB.second);
        Porte* porteB = dynamic_cast<Porte*>(casePorteB);
        if (porteB) porteB->ouvrir();
        
        std::cout << CYAN << "\n[Aventurier A] a tiré un levier ! Un mécanisme s'active dans l'autre monde..." << RESET << std::endl;
        std::cin.get();
    }

    // 2. On regarde la case sous le Joueur B
    Case* caseSousB = donjonB.getCase(joueurB.getX(), joueurB.getY());
    if (caseSousB && caseSousB->afficher() == 'L') {
        Levier* levier = dynamic_cast<Levier*>(caseSousB);
        if (levier) levier->activer();
        
        Case* casePorteA = donjonA.getCase(posPorteA.first, posPorteA.second);
        Porte* porteA = dynamic_cast<Porte*>(casePorteA);
        if (porteA) porteA->ouvrir();
        
        std::cout << CYAN << "\n[Aventurier B] a tiré un levier ! Un mécanisme s'active dans l'autre monde..." << RESET << std::endl;
        std::cin.get();
    }
}

void JeuDuo::boucleDeJeu() {
    char touche;
    bool afficherCheminA = false;
    bool afficherCheminB = false;
    
    auto startTimer = std::chrono::steady_clock::now();
    
    // NOUVEAU : Drapeaux pour savoir si le temps d'un joueur est gelé
    bool chronoA_Stoppe = false; 
    bool chronoB_Stoppe = false; 
    
    while (joueurA.estVivant() && joueurB.estVivant() && !(joueurA.aGagnePartie() && joueurB.aGagnePartie())) {
        
        if (joueurA.aGagnePartie()) joueurAActif = false;
        if (joueurB.aGagnePartie()) joueurAActif = true;

        std::vector<std::pair<int, int>> cheminOptiA;
        std::vector<std::pair<int, int>> cheminOptiB;

        if (afficherCheminA) cheminOptiA = donjonA.trouverChemin(joueurA.getX(), joueurA.getY());
        if (afficherCheminB) cheminOptiB = donjonB.trouverChemin(joueurB.getX(), joueurB.getY());

        afficherSplitScreen(cheminOptiA, cheminOptiB);
        
        std::cout << "\nActions : (ZSQD) Déplacer | (C) Changer de Héros | (P) Chemin | (V) Sauvegarder | (X) Quitter\n>> ";
        std::cin >> touche;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (touche == 'x' || touche == 'X') break;

        if (touche == 'c' || touche == 'C') {
            if (!joueurA.aGagnePartie() && !joueurB.aGagnePartie()) {
                joueurAActif = !joueurAActif;
            } else {
                std::cout << JAUNE << "\nL'autre aventurier vous attend déjà à la sortie !" << RESET << std::endl;
                std::cout << "Appuyez sur [Entree] pour continuer...";
                std::cin.get();
            }
            continue;
        }
        
        if (touche == 'p' || touche == 'P') {
            if (joueurAActif) afficherCheminA = !afficherCheminA;
            else afficherCheminB = !afficherCheminB;
            continue;
        }

        // Sauvegarde Duo mise à jour
        if (touche == 'v' || touche == 'V') {
            auto now = std::chrono::steady_clock::now();
            long long diff = std::chrono::duration_cast<std::chrono::seconds>(now - startTimer).count();
            
            // On n'ajoute le temps qu'à ceux qui n'ont pas encore gagné
            if (!chronoA_Stoppe) joueurA.ajouterTemps(diff);
            if (!chronoB_Stoppe) joueurB.ajouterTemps(diff);
            
            startTimer = now; // On réinitialise pour le prochain tronçon

            std::ofstream ofs("sauvegarde.txt");
            if (ofs.is_open()) {
                this->sauvegarder(ofs);
                std::cout << VERT << "Partie Duo sauvegardée avec succès !" << RESET << std::endl;
                ofs.close();
            }
            std::cin.get();
            continue;
        }

        if (joueurAActif) {
            if (touche == 'z') joueurA.deplacer(0, -1, donjonA);
            else if (touche == 's') joueurA.deplacer(0, 1, donjonA);
            else if (touche == 'q') joueurA.deplacer(-1, 0, donjonA);
            else if (touche == 'd') joueurA.deplacer(1, 0, donjonA);
        } else {
            if (touche == 'z') joueurB.deplacer(0, -1, donjonB);
            else if (touche == 's') joueurB.deplacer(0, 1, donjonB);
            else if (touche == 'q') joueurB.deplacer(-1, 0, donjonB);
            else if (touche == 'd') joueurB.deplacer(1, 0, donjonB);
        }

        verifierMecanismes();

        // --- NOUVEAU : GESTION DYNAMIQUE DES CHRONOS ---
        
        // Si le joueur A vient tout juste de gagner à ce tour
        if (joueurA.aGagnePartie() && !chronoA_Stoppe) {
            auto now = std::chrono::steady_clock::now();
            long long diff = std::chrono::duration_cast<std::chrono::seconds>(now - startTimer).count();
            
            joueurA.ajouterTemps(diff);
            if (!chronoB_Stoppe) joueurB.ajouterTemps(diff); // On synchronise B
            
            startTimer = now; // On redémarre à zéro pour le joueur B restant
            chronoA_Stoppe = true; // Le temps de A est définitivement gelé
        }
        
        // Si le joueur B vient tout juste de gagner à ce tour
        if (joueurB.aGagnePartie() && !chronoB_Stoppe) {
            auto now = std::chrono::steady_clock::now();
            long long diff = std::chrono::duration_cast<std::chrono::seconds>(now - startTimer).count();
            
            if (!chronoA_Stoppe) joueurA.ajouterTemps(diff); // On synchronise A
            joueurB.ajouterTemps(diff);
            
            startTimer = now; // On redémarre à zéro pour le joueur A restant
            chronoB_Stoppe = true; // Le temps de B est définitivement gelé
        }
    }
    
    // Fin de partie globale (mort de l'un ou victoire finale)
    auto endTimer = std::chrono::steady_clock::now();
    long long diff = std::chrono::duration_cast<std::chrono::seconds>(endTimer - startTimer).count();
    
    // On n'ajoute le dernier tronçon qu'à ceux qui étaient encore en train de jouer
    if (!chronoA_Stoppe) joueurA.ajouterTemps(diff);
    if (!chronoB_Stoppe) joueurB.ajouterTemps(diff);
}
void JeuDuo::sauvegarder(std::ofstream& ofs) const {
    ofs << "2\n"; // Marqueur Mode 2 joueurs
    ofs << joueurAActif << "\n";
    joueurA.sauvegarder(ofs);
    donjonA.sauvegarder(ofs);
    joueurB.sauvegarder(ofs);
    donjonB.sauvegarder(ofs);
    ofs << posPorteA.first << " " << posPorteA.second << "\n";
    ofs << posPorteB.first << " " << posPorteB.second << "\n";
}

void JeuDuo::charger(std::ifstream& ifs) {
    ifs >> joueurAActif;
    joueurA.charger(ifs);
    donjonA.charger(ifs);
    joueurB.charger(ifs);
    donjonB.charger(ifs);
    ifs >> posPorteA.first >> posPorteA.second;
    ifs >> posPorteB.first >> posPorteB.second;
}