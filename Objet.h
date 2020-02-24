#pragma once

class Objet {
public:
  Objet(int i = 0, double u = 0.0, double p = 0.0);
  double ratio() const;
  int getId() const;
  double getUtility() const;
  double getWeight() const;

private:
  int id;
  double utilite;
  double poids;
};
