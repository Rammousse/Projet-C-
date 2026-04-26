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
class Monstre : public Case { public: char afficher() override { return 'M'; } };
class Piege : public Case { public: char afficher() override { return 'P'; } };
class Sortie : public Case { public: char afficher() override { return 'S'; } };

// Factory pour créer les cases
class CaseFactory {
public:
    static Case* creerCase(TypeCase type);
};

#endif