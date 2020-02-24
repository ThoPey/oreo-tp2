#include "Objet.h"

Objet::Objet(int i, double u, double p) : id(i), utilite(u), poids(p) {}

double Objet::ratio() const {
  return this->utilite / this->poids;
}

int Objet::getId() const {
  return this->id;
}

double Objet::getUtility() const {
  return this->utilite;
}

double Objet::getWeight() const {
  return this->poids;
}
