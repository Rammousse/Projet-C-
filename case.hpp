#ifndef CASE_HPP
#define CASE_HPP

#include <iostream>

// Types de cases disponibles
enum class TypeCase { MUR, PASSAGE, TRESOR, MONSTRE, PIEGE, SORTIE };

// Classe de base abstraite
class Case {
public:
    virtual char afficher() = 0;   
    virtual ~Case() = default;     
};

// Classes dérivées
class Mur : public Case { public: char afficher() override { return '#'; } };
class Passage : public Case { public: char afficher() override { return ' '; } };
class Tresor : public Case { public: char afficher() override { return 'T'; } };
class Piege : public Case { public: char afficher() override { return 'P'; } };
class Sortie : public Case { public: char afficher() override { return 'S'; } };

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

// Factory pour créer les cases
class CaseFactory {
public:
    static Case* creerCase(TypeCase type);
};

#endif