#include "case.hpp"

/**
 * Fabrique des cases utilisant une structure de contrôle switch, centralise l'instanciation des objets dérivés de Case, 
 * facilitant l'ajout de nouveaux types de terrain sans modifier tout le code
 * Case* : pointeur d'objet ou nullptr si le type est inconnu
 */
Case* CaseFactory::creerCase(TypeCase type) {
    switch (type) {
        case TypeCase::MUR:     return new Mur();
        case TypeCase::PASSAGE: return new Passage();
        case TypeCase::TRESOR:  return new Tresor();
        case TypeCase::MONSTRE: return new Monstre();
        case TypeCase::PIEGE:   return new Piege();
        case TypeCase::PORTE:   return new Porte();
        case TypeCase::LEVIER:  return new Levier();
        case TypeCase::SORTIE:  return new Sortie();
        default: return nullptr;
    }
}