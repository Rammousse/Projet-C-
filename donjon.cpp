#include "donjon.hpp"
#include "case.hpp"
#include <algorithm> // pour std::random_shuffle
#include <iostream>
#include <vector>

Donjon::Donjon() : largeur(0), hauteur(0) {}

Donjon::~Donjon() {
    for (auto& ligne : grille) {
        for (Case* c : ligne) {
            delete c;
        }
    }
}

// Initialisation : remplit tout de MURS
void Donjon::generer(int l, int h) {
    largeur = l;
    hauteur = h;
    grille.assign(hauteur, std::vector<Case*>(largeur, nullptr));

    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            grille[y][x] = CaseFactory::creerCase(TypeCase::MUR);
        }
    }

    // Lancement de la génération récursive à partir de (1, 1)
    genererLabyrinthe(1, 1);
    
    // Placement de l'entrée et de la sortie
    delete grille[1][1]; grille[1][1] = CaseFactory::creerCase(TypeCase::PASSAGE);

    //sortie
    int outX = largeur - 2;
    int outY = hauteur - 2;
    delete grille[outY][outX];
    grille[outY][outX] = CaseFactory::creerCase(TypeCase::SORTIE);
}

void Donjon::genererLabyrinthe(int x, int y) {
    // Remplacer le mur actuel par un passage
    delete grille[y][x];
    grille[y][x] = CaseFactory::creerCase(TypeCase::PASSAGE);

    // Directions : NORD, SUD, EST, OUEST
    std::vector<std::pair<int, int>> directions = {{0, -2}, {0, 2}, {2, 0}, {-2, 0}};
    std::random_shuffle(directions.begin(), directions.end()); // Aléatoire [cite: 459]

    for (auto& d : directions) {
        int nx = x + d.first;
        int ny = y + d.second;

        // Vérifier si le voisin (à 2 cases) est dans les bornes et est encore un MUR
        if (nx > 0 && nx < largeur - 1 && ny > 0 && ny < hauteur - 1) {
            if (grille[ny][nx]->afficher() == '#') {
                // Casser le mur entre (x,y) et (nx,ny) 
                int murX = x + d.first / 2;
                int murY = y + d.second / 2;
                delete grille[murY][murX];
                grille[murY][murX] = CaseFactory::creerCase(TypeCase::PASSAGE);

                // Appel récursif 
                genererLabyrinthe(nx, ny);
            }
        }
    }
}

void Donjon::afficher(int playerX, int playerY) {
    system("clear"); // Nettoie l'écran pour un effet "jeu"

    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            if (x == playerX && y == playerY) {
                // Joueur en Vert
                std::cout << VERT << "@ " << RESET;
            } else {
                char symbole = grille[y][x]->afficher();
                
                switch (symbole) {
                    case 'T': // Trésor en Jaune
                        std::cout << JAUNE << "T " << RESET;
                        break;
                    case 'M': // Monstre en Rouge
                        std::cout << ROUGE << "M " << RESET;
                        break;
                    case 'P': // Piège en Cyan (bleu clair)
                        std::cout << CYAN << "P " << RESET;
                        break;
                    case '#': // Murs en blanc/gris (normal)
                        std::cout << "# ";
                        break;
                    case 'S':
                        std::cout << "\033[34m" << "S " << "\033[0m"; // Bleu
                        break;
                    default: // Passages
                        std::cout << "  "; 
                        break;
                }
            }
        }
        std::cout << std::endl;
    }
}

Case* Donjon::getCase(int x, int y) {
    if (x < 0 || x >= largeur || y < 0 || y >= hauteur) return nullptr;
    return grille[y][x];
}