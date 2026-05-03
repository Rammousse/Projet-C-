#include <iostream>
#include <fstream>
#include <ctime>
#include "donjon.hpp"
#include "aventurier.hpp"
#include "jeuduo.hpp" 

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    srand(static_cast<unsigned>(time(nullptr)));

    std::cout << "============================" << std::endl;
    std::cout << "   BIENVENUE DANS LE DONJON " << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "Objectif : Trouver la sortie et survivre !" << std::endl;
    std::cout << " " << std::endl;
    
    std::cout << "1. Nouvelle partie" << std::endl;
    std::cout << "2. Charger partie" << std::endl;
    std::cout << "Choix : ";

    int choixPrincipal;
    std::cin >> choixPrincipal;
    
    if (choixPrincipal == 1) {
        std::cout << "\nMODE DE JEU :" << std::endl;
        std::cout << "  1. 1 joueur" << std::endl;
        std::cout << "  2. 2 joueurs" << std::endl;
        std::cout << "Choix : ";
        
        int choixMode;
        std::cin >> choixMode;
        
        if (choixMode == 1) {
            Donjon monDonjon;
            Aventurier joueur(1, 1);
            std::cout << "Génération du labyrinthe en cours..." << std::endl;
            monDonjon.generer(21, 11);
            joueur.boucleDeJeu(monDonjon);
            
            int dist = monDonjon.trouverChemin(1,1).size() > 0 ? monDonjon.trouverChemin(1,1).size() - 1 : 0;
            joueur.afficherRapportFinal(dist);
            
        } else if (choixMode == 2) {
            JeuDuo modeDuo;
            std::cout << "Génération des mondes liés en cours..." << std::endl;
            modeDuo.genererMondes();
            modeDuo.boucleDeJeu();
            
            // Affichage des deux rapports
            modeDuo.getJoueurA().afficherRapportFinal(modeDuo.getDistanceOptimaleA());
            std::cout << "\n\n---> Appuyez sur [Entrée] pour voir le rapport du Joueur B <---";
            std::cin.get();
            modeDuo.getJoueurB().afficherRapportFinal(modeDuo.getDistanceOptimaleB());
        }
        
    } else if (choixPrincipal == 2) {
        std::ifstream ifs("sauvegarde.txt");
        if (ifs.is_open()) {
            int mode;
            ifs >> mode; // On lit le marqueur caché !
            
            if (mode == 1) {
                Donjon monDonjon;
                Aventurier joueur(1, 1);
                joueur.charger(ifs);     
                monDonjon.charger(ifs);
                ifs.close();
                
                std::cout << "Partie Solo chargée !" << std::endl;
                joueur.boucleDeJeu(monDonjon);
                
                int dist = monDonjon.trouverChemin(1,1).size() > 0 ? monDonjon.trouverChemin(1,1).size() - 1 : 0;
                joueur.afficherRapportFinal(dist);
                
            } else if (mode == 2) {
                JeuDuo modeDuo;
                modeDuo.charger(ifs);
                ifs.close();
                
                std::cout << "Partie Duo chargée !" << std::endl;
                modeDuo.boucleDeJeu();
                
                modeDuo.getJoueurA().afficherRapportFinal(modeDuo.getDistanceOptimaleA());
                std::cout << "\n\n---> Appuyez sur [Entrée] pour voir le rapport du Joueur B <---";
                std::cin.get();
                modeDuo.getJoueurB().afficherRapportFinal(modeDuo.getDistanceOptimaleB());
            }
        } else {
            std::cout << "Aucune sauvegarde trouvée !" << std::endl;
        }
    }

    return 0;
}