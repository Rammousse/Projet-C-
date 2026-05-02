#include "donjon.hpp"
#include "case.hpp"
#include <algorithm> // pour std::random_shuffle
#include <random> // pour std::random_device rd;
#include <chrono>
#include <iostream>
#include <vector>
#include <queue>
#include <map>

std::vector<std::pair<int, int>> Donjon::trouverChemin(int startX, int startY) {
    int endX = largeur - 2, endY = hauteur - 2;

    // File pour le BFS : stocke les coordonnées (x, y)
    std::queue<std::pair<int, int>> file;
    file.push({startX, startY});

    // Pour stocker d'où on vient : parent[{filsX, filsY}] = {parentX, parentY}
    std::map<std::pair<int, int>, std::pair<int, int>> parents;
    parents[{startX, startY}] = {-1, -1}; // Le point de départ n'a pas de parent

    bool sortieTrouvee = false;

    // Directions possibles (Haut, Bas, Gauche, Droite)
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (!file.empty()) {
        std::pair<int, int> courant = file.front();
        file.pop();

        if (courant.first == endX && courant.second == endY) {
            sortieTrouvee = true;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nx = courant.first + dx[i];
            int ny = courant.second + dy[i];

            // Si la case est valide, n'est pas un mur, et n'a pas encore été visitée
            if (nx >= 0 && nx < largeur && ny >= 0 && ny < hauteur &&
                grille[ny][nx]->afficher() != '#' &&
                parents.find({nx, ny}) == parents.end()) {
                
                parents[{nx, ny}] = courant;
                file.push({nx, ny});
            }
        }
    }

    // Reconstruction du chemin si la sortie a été trouvée
    std::vector<std::pair<int, int>> chemin;
    if (sortieTrouvee) {
        std::pair<int, int> actuel = {endX, endY};
        while (actuel.first != -1) {
            chemin.push_back(actuel);
            actuel = parents[actuel];
        }
        std::reverse(chemin.begin(), chemin.end());
    }
    return chemin;
}



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

    placerElements();
    
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
    
    // On crée un générateur de nombres aléatoires basé sur le temps
    // On utilise l'horloge système au lieu de random_device
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 g(seed);   

    // On utilise shuffle à la place de random_shuffle
    std::shuffle(directions.begin(), directions.end(), g);

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

void Donjon::afficher(int playerX, int playerY, const std::vector<std::pair<int, int>>& chemin) {
    system("clear"); 

    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            
            // 1. Priorité au joueur
            if (x == playerX && y == playerY) {
                std::cout << VERT << "@ " << RESET;
                continue;
            }

            // 2. Vérifier si cette case est dans le chemin optimal
            bool estChemin = false;
            for (auto const& coord : chemin) {
                if (coord.first == x && coord.second == y) {
                    estChemin = true;
                    break;
                }
            }

            // 3. Affichage
            if (estChemin && grille[y][x]->afficher() == ' ') {
                // On affiche un point bleu pour le chemin
                std::cout << BLEU << ". " << RESET;
            } else {
                char symbole = grille[y][x]->afficher();
                switch (symbole) {
                    case 'T': std::cout << JAUNE << "T " << RESET; break;
                    case 'M': std::cout << ROUGE << "M " << RESET; break;
                    case 'P': std::cout << CYAN << "P " << RESET; break;
                    case 'S': std::cout << BLEU << "S " << RESET; break; // La sortie
                    case '#': std::cout << "# "; break;
                    default:  std::cout << "  "; break;
                }
            }
        }
        std::cout << std::endl;
    }
}

void Donjon::placerElements() {
    for (int y = 1; y < hauteur - 1; ++y) {
        for (int x = 1; x < largeur - 1; ++x) {
            // On ne place des éléments QUE sur les cases PASSAGE (les cases vides)
            // Et on évite la case de départ (1,1) pour ne pas tuer le joueur direct !
            if (grille[y][x]->afficher() == ' ' && !(x == 1 && y == 1)) {
                
                int proba = rand() % 100; // Nombre entre 0 et 99

                if (proba < 10) { // 10% de chance d'avoir un Trésor
                    delete grille[y][x];
                    grille[y][x] = CaseFactory::creerCase(TypeCase::TRESOR);
                } 
                else if (proba < 20) { // 10% de chance d'avoir un Monstre (entre 10 et 19)
                    delete grille[y][x];
                    grille[y][x] = CaseFactory::creerCase(TypeCase::MONSTRE);
                } 
                else if (proba < 25) { // 5% de chance d'avoir un Piège (entre 20 et 24)
                    delete grille[y][x];
                    grille[y][x] = CaseFactory::creerCase(TypeCase::PIEGE);
                }
            }
        }
    }
}

void Donjon::setCase(int x, int y, TypeCase type) {
    if (x >= 0 && x < largeur && y >= 0 && y < hauteur) {
        delete grille[y][x];
        grille[y][x] = CaseFactory::creerCase(type);
    }
}

Case* Donjon::getCase(int x, int y) {
    if (x < 0 || x >= largeur || y < 0 || y >= hauteur) return nullptr;
    return grille[y][x];
}