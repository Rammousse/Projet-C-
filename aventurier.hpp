#ifndef AVENTURIER_HPP
#define AVENTURIER_HPP

#include <fstream>
#include <set>
#include <utility>
#include "donjon.hpp"

/**
 * Classe de l'aventurier (le joueur)
 * gere ses stats, son inventaire et ou il est sur la grille
 */
class Aventurier {
private:
    int posX, posY;
    int pv;
    int inventaire; // nb de tresors rammassés
    bool aGagne; // flag pour savoir si c la fin

    // variables pour calculer le score final
    int pasEffectues;
    int combatsGagnes;
    int piegesSubis;
    long long tempsCumule; // temps en sec
    std::set<std::pair<int, int>> casesVisitees;

public:
    /**
     * Constructeur avec la pos de depart
     */
    Aventurier(int startX, int startY);

    // Getters de base
    int getX() const { return posX; }
    int getY() const { return posY; }
    int getPv() const { return pv; }
    int getInventaire() const { return inventaire; }
    void ajouterTemps(long long t) { tempsCumule += t; }

    /**
     * Deplace le joueur s'il n'y a pas de mur devant
     */
    void deplacer(int dx, int dy, Donjon& d);

    /**
     * Check l'effet de la nvlle case (tresor, piege, monstre...)
     */
    void resoudreCase(Case* c, Donjon& d);

    bool estVivant() const;

    /**
     * Affiche la barre de vie et le nbr de tresors en temps reel
     */
    void afficherStatut() const;
    
    /**
     * La boucle pricipale (lit le clavier zqsd, gere le menu et chrono)
     */
    void boucleDeJeu(Donjon& d);

    /**
     * Ecrit/Lit l'etat du joueur dans le .txt
     */
    void sauvegarder(std::ofstream& ofs) const;
    void charger(std::ifstream& ifs);

    /**
     * Lance l'interface du combat au tour par tour contre un mob
     */
    void engagerCombat(Monstre* m, Donjon& d);
    
    void afficherBarreVie(int pvActuels, int pvMax, const std::string& nom) const;
    
    // prend des degats et evite de passer en pv negatifs
    void recevoirDegats(int d) { pv -= d; if(pv < 0) pv = 0; }
    
    /**
     * Affiche l'ecran de fin avec le score et l'efficacité du trajet opti
     */
    void afficherRapportFinal(int cheminMinimal) const;

    bool aGagnePartie() const { return aGagne; }
};

#endif