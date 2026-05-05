#include "donjon.hpp"
#include "case.hpp"
#include <algorithm> // pour std::random_shuffle
#include <random> // pour std::random_device rd;
#include <chrono>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>

/**
 * Algorithme de recherche en largeur (BFS) pour trouver le chemin le plus court vers la sortie
 */
std::vector<std::pair<int, int>> Donjon::trouverChemin(int startX, int startY) {
    int endX = largeur - 2, endY = hauteur - 2;

    std::queue<std::pair<int, int>> file;
    file.push({startX, startY}); //stocke les coordonnées x et y

    // Pour stocker d'où on vient (parents)pour la reconstruction du chemin
    std::map<std::pair<int, int>, std::pair<int, int>> parents;
    parents[{startX, startY}] = {-1, -1}; // Le point de départ n'a pas de parent

    bool sortieTrouvee = false;

    // Directions possibles (Haut, Bas, Gauche, Droite)
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (!file.empty()) {
        std::pair<int, int> courant = file.front();
        file.pop();

        if (courant.first == endX && courant.second == endY) { // si on a atteint les coordonnées de la sortie
            sortieTrouvee = true;
            break;
        }

        for (int i = 0; i < 4; i++) { // explore les 4 voisins adjacents
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

    // Reconstruction du chemin si la sortie a été trouvée (si le bfs a reussi)
    std::vector<std::pair<int, int>> chemin;
    if (sortieTrouvee) {
        std::pair<int, int> actuel = {endX, endY};
        while (actuel.first != -1) {
            chemin.push_back(actuel);
            actuel = parents[actuel]; // on remonte vers le depart
        }
        std::reverse(chemin.begin(), chemin.end()); // On remet dans l'ordre
    }
    return chemin;
}



Donjon::Donjon() : largeur(0), hauteur(0) {}

Donjon::~Donjon() { //libère la mémoire de chaque objet Case dynamiquement alloué (destructeur)
    for (auto& ligne : grille) {
        for (Case* c : ligne) {
            delete c;
        }
    }
}


/**
 * Prépare la structure du donjon et lance la génération
 */

// Initialisation : remplit tout de murs
void Donjon::generer(int l, int h) {
    largeur = l;
    hauteur = h;
    grille.assign(hauteur, std::vector<Case*>(largeur, nullptr));  // initialise la grille avec des pointeurs nuls

    // On remplit tout de murs (blocs pleins)
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            grille[y][x] = CaseFactory::creerCase(TypeCase::MUR);
        }
    }

    // Lancement de la génération récursive à partir de (1, 1), creuse le labyrinthe
    genererLabyrinthe(1, 1);

    placerElements(); // ajoute les objets (monstres, trésors...)
    
    // Placement de l'entrée 
    delete grille[1][1]; grille[1][1] = CaseFactory::creerCase(TypeCase::PASSAGE);

    //sortie (en bas a droite)
    int outX = largeur - 2;
    int outY = hauteur - 2;
    delete grille[outY][outX];
    grille[outY][outX] = CaseFactory::creerCase(TypeCase::SORTIE);
}

/**
 * Algorithme de génération procédurale (Recursive Backtracker)
 */

void Donjon::genererLabyrinthe(int x, int y) {
    // Remplacer le mur actuel par un passage
    delete grille[y][x];
    grille[y][x] = CaseFactory::creerCase(TypeCase::PASSAGE);

    // directions avec un saut de 2 cases (pour laisser un mur entre les couloirs)
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

        // Vérifier si le voisin (à 2 cases) est dans les bornes et est encore un mur
        if (nx > 0 && nx < largeur - 1 && ny > 0 && ny < hauteur - 1) {
            if (grille[ny][nx]->afficher() == '#') {
                // Casser le mur entre (x,y) et (nx,ny) 
                int murX = x + d.first / 2;
                int murY = y + d.second / 2;
                delete grille[murY][murX];
                grille[murY][murX] = CaseFactory::creerCase(TypeCase::PASSAGE);

                // Appel récursif, continue l'exploration depuis la nouvelle case
                genererLabyrinthe(nx, ny);
            }
        }
    }
}

/**
 * Affiche le donjon dans la console avec des couleurs
 */

void Donjon::afficher(int playerX, int playerY, const std::vector<std::pair<int, int>>& chemin) {
    system("clear"); // Efface la console pour l'animation

    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            
            // priorité au joueur
            if (x == playerX && y == playerY) {
                std::cout << VERT << "@ " << RESET;
                continue;
            }

            // Verifie si cette case est dans le chemin optimal (BFS)
            bool estChemin = false;
            for (auto const& coord : chemin) {
                if (coord.first == x && coord.second == y) {
                    estChemin = true;
                    break;
                }
            }

            // Affichage selon le type de case
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

/**
 * Distribue aléatoirement des éléments interactifs dans le labyrinthe
 */

void Donjon::placerElements() {
    for (int y = 1; y < hauteur - 1; ++y) {
        for (int x = 1; x < largeur - 1; ++x) {
            // On ne place des éléments QUE sur les cases passage (les cases vides)
            // Et on évite la case de départ (1,1) pour ne pas tuer le joueur direct
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

//modifie manuellement une case spécifique
void Donjon::setCase(int x, int y, TypeCase type) {
    if (x >= 0 && x < largeur && y >= 0 && y < hauteur) {
        delete grille[y][x];
        grille[y][x] = CaseFactory::creerCase(type);
    }
}

//Récupère le pointeur de la case à une position donnée
Case* Donjon::getCase(int x, int y) {
    if (x < 0 || x >= largeur || y < 0 || y >= hauteur) return nullptr;
    return grille[y][x];
}

//ecrit l'état actuel de la grille dans un fichier
void Donjon::sauvegarder(std::ofstream& ofs) const {
    ofs << largeur << " " << hauteur << "\n";
    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            ofs << grille[y][x]->afficher(); // Sauvegarde le caractère ('#', ' ', 'M'...)
        }
        ofs << "\n";
    }
}

void Donjon::charger(std::ifstream& ifs) { // lit un fichier de sauvegarde et recrée les objets Case correspondants
    ifs >> largeur >> hauteur;
    ifs.ignore(); // Ignorer le saut de ligne après les dimensions

    // Nettoyage de l'ancienne mémoire avant le chargement
    for (auto& ligne : grille) {
        for (Case* c : ligne) delete c;
    }
    grille.assign(hauteur, std::vector<Case*>(largeur, nullptr));

    std::string ligne;
    for (int y = 0; y < hauteur; ++y) {
        std::getline(ifs, ligne);
        for (int x = 0; x < largeur; ++x) {
            char symbole = ligne[x];
            TypeCase type;
            // Détermine le type d'objet à créer selon le caractère lu
            switch(symbole) {
                case '#': type = TypeCase::MUR; break;
                case 'T': type = TypeCase::TRESOR; break;
                case 'M': type = TypeCase::MONSTRE; break;
                case 'P': type = TypeCase::PIEGE; break;
                case 'S': type = TypeCase::SORTIE; break;
                case 'D': type = TypeCase::PORTE; break;
                case 'L': type = TypeCase::LEVIER; break;
                case 'l': type = TypeCase::LEVIER; break; // Levier activé
                default:  type = TypeCase::PASSAGE; break; // Espace ' '
            }
            grille[y][x] = CaseFactory::creerCase(type);
        }
    }
}

//version texte formatée d'une ligne pour l'affichage (utilisée pour l'UI)
std::string Donjon::getLigneAffichage(int y, int playerX, int playerY, const std::vector<std::pair<int, int>>& chemin) {
    std::string ligne = "";
    for (int x = 0; x < largeur; ++x) {
        if (x == playerX && y == playerY) {
            ligne += VERT + "@ " + RESET;
        } else {
            // Vérification du chemin optimal
            bool estChemin = false;
            for (auto const& coord : chemin) {
                if (coord.first == x && coord.second == y) {
                    estChemin = true;
                    break;
                }
            }
            
            if (estChemin && grille[y][x]->afficher() == ' ') {
                ligne += BLEU + ". " + RESET;
            } else {
                char symbole = grille[y][x]->afficher();
                switch (symbole) {
                    case 'T': ligne += JAUNE + "T " + RESET; break;
                    case 'M': ligne += ROUGE + "M " + RESET; break;
                    case 'P': ligne += CYAN + "P " + RESET; break;
                    case 'S': ligne += BLEU + "S " + RESET; break;
                    case 'D': ligne += MAGENTA + "D " + RESET; break;
                    case 'L': ligne += MAGENTA + "L " + RESET; break;
                    case 'l': ligne += VERT + "l " + RESET; break;
                    case '#': ligne += "# "; break;
                    default:  ligne += "  "; break;
                }
            }
        }
    }
    return ligne;
}