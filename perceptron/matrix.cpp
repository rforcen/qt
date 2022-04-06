#include "matrix.h"

double testDbl() { return rand(); }

void testMatrixClass() {
  const int n = 5;

  Matrix m(n, n, Vector::rand01), m1;

  Vector v(n, Vector::rand01), vd(n), b(n), vt(n);

  m.random();
  v.random();

  m1 = m;
  double s = m.sum();
  s = (m1 - m).sum();
  assert(s == 0);

  v = 2;
  b = 3;
  vt = v * vd + b;

  m = 1;
  v = 2;
  b = 3;

  vt = (m * v) + b;
  for (int i = 0; i < 100; i++) vd = (m * v) + b;
  bool eq = (vt == vd);

  assert(eq);

  m = 0;
  m.identity();

  double det = 0;

  m.random();
  m1 = m;
  det = m.det();

  eq = (m == m1);
  assert(eq);
  v = vd;
}

void testVector() {
  //  for (int i = 0; i < 10000; i++) testPrt();

  QString s;
  Vector v1(10, 0.), v2(100, 34.56), v3(30, 12.34), v4, v5;
  VectorRnd vr(10);
  Vector vv = {1, 2, 3};

  v4 = v2;
  v5 = v2;

  v2[0] = 123;  // creates a data copy

  v1.func(Vector::rand01);

  double sum = 0, stest;
  for (auto d : vr) sum += d;
  stest = vr.sum();

  Q_ASSERT(stest == sum);

  Vector& vSort = vr.sort();  // creates a ref. copy
  vr[0] = 12;                 // this modifies vSort also

  auto max = vr.max(), min = vr.min();
  //  auto mm = vr.minmax();
  vSort.scale();

  v1.range(min, max, 0.04).func(Vector::sigmoid);

  VectorRnd vs(1000);
  vs.func(Vector::sigmoid);

  for (int i = 0; i < 1000; i++) {
    v2 = 56.78;
    v1 = v3 = v2;
    v3 += v1;

    bool b1 = v1 == v2, b2 = (v1 + 56.78) == v3;

    Q_ASSERT(b1);
    Q_ASSERT(b2);

    v4 += v1 * v2 + v3;
    v5 += v3 + v1 * v2;

    Vector vOne(v4, 1.);
    Vector v7 = vOne / v4;
    Vector v8 = Vector(v4.size(), 1.) / v4;

    Q_ASSERT(v4 == v5);
    Q_ASSERT(v7 == v8);

    s = v4.toString();
  }
}
