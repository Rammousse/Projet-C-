#include "case.hpp"

Case* CaseFactory::creerCase(TypeCase type) {
    switch (type) {
        case TypeCase::MUR:     return new Mur();
        case TypeCase::PASSAGE: return new Passage();
        case TypeCase::TRESOR:  return new Tresor();
        case TypeCase::MONSTRE: return new Monstre();
        case TypeCase::PIEGE:   return new Piege();
        case TypeCase::SORTIE:  return new Sortie();
        default: return nullptr;
    }
}