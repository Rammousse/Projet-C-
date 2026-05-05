#ifndef CASE_HPP
#define CASE_HPP

#include <iostream>

/**
 * Enumération des différents types de cases disponibles dans le donjon
 */

// Types de cases disponibles
enum class TypeCase { MUR, PASSAGE, TRESOR, MONSTRE, PIEGE, PORTE, LEVIER, SORTIE };

/**
 * Case : classe de base abstraite représentant une unité de la grille du donjon
 */
class Case {
public:
    virtual char afficher() = 0;   // retourne le caractère représentant la case dans la console.
    virtual ~Case() = default;     // destructeur virtuel pour assurer une libération propre en polymorphisme
};

// Classes dérivées

/** représente un mur ('#') */
class Mur : public Case { public: char afficher() override { return '#'; } };

/** représente un couloir ou sol vide (' ')*/
class Passage : public Case { public: char afficher() override { return ' '; } };

/** représente un trésor ('T')*/
class Tresor : public Case { public: char afficher() override { return 'T'; } };

/** représente un piège au sol ('P') */
class Piege : public Case { public: char afficher() override { return 'P'; } };

/** représente la sortie du donjon ('S') */
class Sortie : public Case { public: char afficher() override { return 'S'; } };

/**
 * Monstre : représente un ennemi avec des statistiques de combat
 */
class Monstre : public Case { 
private:
    int pv;
    int degats;
public: 
    // Constructeur : 50 PV et 15 Dégâts par défaut
    Monstre() : pv(50), degats(15) {} 
    
    char afficher() override { return 'M'; } 
    
    // Getters et utilitaires de combat
    int getPv() const { return pv; }
    int getDegats() const { return degats; }
    void recevoirDegats(int d) { 
        pv -= d; 
        if(pv < 0) pv = 0; 
    }
    bool estVivant() const { return pv > 0; }
};

/**
 * Classe Porte : représente un passage qui peut être fermé ou ouvert
 */
class Porte : public Case {
private:
    bool ouverte = false;
public:
    void ouvrir() { ouverte = true; }
    bool estOuverte() const { return ouverte; }
    char afficher() override { return ouverte ? ' ' : 'D'; } // D pour Door
};

/**
 * Classe Levier : represente un mécanisme pouvant être activé (lié à une porte)
 */
class Levier : public Case {
private:
    bool active = false;
public:
    void activer() { active = true; }
    bool estActive() const { return active; }
    char afficher() override { return active ? 'l' : 'L'; } // L majuscule = non tiré
};

// Factory pour créer les cases

/**
 * CaseFactory : implémentation du Design Pattern Factory pour la création de cases
 */
class CaseFactory {
public:
    /**
     * Crée dynamiquement une instance de Case selon le type demandé, avec Case* Un pointeur vers l'objet créé 
    static Case* creerCase(TypeCase type);
};

#endif