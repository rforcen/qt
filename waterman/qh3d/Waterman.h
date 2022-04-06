#pragma once

#include "Point3d.h"
#include "QuickHull3D.h"

class WatermanPoly {

public:
  QuickHull3D *hull = nullptr;

  WatermanPoly();
  ~WatermanPoly();
  vector<double> genPoly(double radius);
  void genHull(double radius);

  bool ok = true;
};
