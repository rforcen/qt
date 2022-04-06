#ifndef MATRIX_H
#define MATRIX_H

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <limits>
#include <memory>

#include <QString>

class Vector {
 public:  // methods
  Vector() : n(0), v(nullptr) {}
  Vector(int n) { set(n); }
  Vector(int n, double d) { set(n, d); }
  Vector(int n, double (*fd)()) {
    set(n);
    func(fd);
  }
  Vector(int n, double* v) {
    this->n = n;
    this->v = v;
  }
  Vector(Vector& vv, double d) { set(vv.n, d); }
  Vector(std::initializer_list<double> li) {  //  list init->copy
    _alloc(li.size());
    int i = 0;
    for (auto l : li) v[i++] = l;
  }

  ~Vector() { Vector::_free(); }

  int size() const { return n; }

  double* begin() { return &v[0]; }
  double* end() { return &v[n]; }

  void set(int n) {
    Vector::_realloc(n);
    for (int i = 0; i < n; i++) v[i] = 0;
  }
  void set(int n, double d) {
    Vector::_realloc(n);
    for (int i = 0; i < n; i++) v[i] = d;
  }
  void _alloc(int n) {
    this->n = n;

    if (n > 0) {
      this->v = new double[n];
      allocated = new bool[1];
      *allocated = true;
    } else
      this->v = nullptr;
  }
  void _free() {
    if (allocated && *allocated) {  //  don't free twice
      delete[] v;
      *allocated = false;
    }
    n = 0;
    v = nullptr;
  }
  void _realloc(int n) {
    if (this->n != n) {
      Vector::_free();
      Vector::_alloc(n);
    }
  }
  Vector& copy(Vector& v) {
    _realloc(v.n);
    for (int i = 0; i < n; i++) this->v[i] = v[i];
    return *this;
  }
  void random() {
    for (int i = 0; i < n; i++) v[i] = rand01();
  }
  void func(double (*fd)()) {
    for (int i = 0; i < n; i++) v[i] = fd();
  }
  void func(double (*fd)(double x)) {
    for (int i = 0; i < n; i++) v[i] = fd(v[i]);
  }

  void func(int (*fd)()) {
    for (int i = 0; i < n; i++) v[i] = fd();
  }

  void func(float (*fd)()) {
    for (int i = 0; i < n; i++) v[i] = fd();
  }
  void func(float (*fd)(float x)) {
    for (int i = 0; i < n; i++) v[i] = fd((float)v[i]);
  }

  Vector& operator=(double* d) {
    n = sizeof(d) / sizeof(double);
    v = d;
    return *this;
  }

  Vector& operator=(const Vector& a) {  // a=b, make a same size as b
    Vector::_realloc(a.size());

    for (int i = 0; i < n; i++) v[i] = a[i];
    return *this;
  }
  Vector& operator=(const double& d) {
    for (int i = 0; i < n; i++) this->v[i] = d;
    return *this;
  }
  inline double& operator[](const int i) { return v[i]; }  // v[i]=0;
  inline const double& operator[](const int i) const { return v[i]; }

  Vector& operator*=(const Vector& a) {
    assert(n == a.n);
    for (int i = 0; i < n; i++) v[i] *= a[i];
    return *this;
  }
  Vector& operator+=(const Vector& a) {
    assert(n == a.n);
    for (int i = 0; i < n; i++) v[i] += a[i];
    return *this;
  }
  Vector& operator-=(const Vector& a) {
    assert(n == a.n);
    for (int i = 0; i < n; i++) v[i] -= a[i];
    return *this;
  }

  Vector operator*(const Vector& a) {
    assert(n == a.n);
    Vector vr(n);
    for (int i = 0; i < n; i++) vr[i] = a[i] * v[i];
    return vr;
  }
  Vector operator*(const double a) {
    Vector vr(n);
    for (int i = 0; i < n; i++) vr[i] = v[i] * a;
    return vr;
  }
  Vector operator/(const Vector& a) {
    assert(n == a.n);
    Vector vr(n);
    for (int i = 0; i < n; i++) vr[i] = a[i] / v[i];
    return vr;
  }
  Vector operator/(const double a) {
    Vector vr(n);
    for (int i = 0; i < n; i++) vr[i] = v[i] / a;
    return vr;
  }
  Vector operator+(const Vector& a) {
    assert(n == a.n);
    Vector vr(n);
    for (int i = 0; i < n; i++) vr[i] = a[i] + v[i];
    return vr;
  }
  Vector operator+(const double& a) {
    Vector vr(n);
    for (int i = 0; i < n; i++) vr[i] = v[i] + a;
    return vr;
  }
  Vector operator-(const Vector& a) {
    assert(n == a.n);
    Vector vr(n);
    for (int i = 0; i < n; i++) vr[i] = v[i] - a[i];
    return vr;
  }
  Vector operator-(const double& a) {
    Vector vr(n);
    for (int i = 0; i < n; i++) vr[i] = v[i] - a;
    return vr;
  }
  bool operator==(const Vector& a) {
    assert(n == a.n);
    for (int i = 0; i < n; i++)
      if (a[i] != v[i]) return false;
    return true;
  }
  bool operator!=(const Vector& a) {
    assert(n == a.n);
    for (int i = 0; i < n; i++)
      if (a[i] != v[i]) return true;
    return false;
  }

  double sum() {
    double s = 0;
    for (int i = 0; i < n; i++) s += v[i];
    return s;
  }
  Vector& sort() {
    std::sort(begin(), end());
    return *this;
  }
  double max() { return *std::max(begin(), end()); }
  double min() { return *std::min(begin(), end()); }
  auto minmax() { return std::minmax_element(begin(), end()); }
  void scale() {
    auto mm = minmax();
    double min = v[mm.first - begin()], max = v[mm.second - begin()];
    double d = fabs(max - min);
    if (d == 0) d = min;
    if (d)
      for (auto& vv : *this) vv = (vv - min) / d;
  }

  double dot(const Vector& a) {
    assert(n == a.size());

    double dp = 0;
    for (int i = 0; i < n; i++) dp += v[i] * a[i];
    return dp;
  }

  double mod(void) {
    double m = 0;
    for (int i = 0; i < n; i++) m += v[i] * v[i];
    return sqrt(m);
  }
  double distance(void) { return mod(); }
  Vector& zero() {
    for (int i = 0; i < n; i++) v[i] = 0;
    return *this;
  }

  Vector& range(int n, double start, double delta) {
    Vector::_realloc(n);
    for (int i = 0; i < n; i++, start += delta) v[i] = start;
    return *this;
  }
  Vector& range(double start, double end, double delta) {
    int ni = (int)((end - start) / delta);
    assert(ni > 0);
    Vector::_realloc(ni);
    for (int i = 0; start < end && i < n; i++, start += delta) v[i] = start;
    return *this;
  }

  QString toString() {
    QString s;
    for (int i = 0; i < n; i++) s += QString::number(v[i]) + "\t";
    return s;
  }

 private:  // props.
  int n = 0;
  double* v = nullptr;
  bool* allocated = nullptr;  //  control double free issue

 public:
  static double rand01() { return (double)rand() / RAND_MAX; }
  static double sigmoid(double z) { return 1. / (1 + exp(-z)); }
};

class VectorRnd : public Vector {
 public:
  VectorRnd(int n) : Vector(n, rand01) {}
};

class Matrix {
 public:
  Matrix() : m(0), v(nullptr) {}
  Matrix(Matrix& mtx) : m(0) {  // create a copy of mtx
    set(mtx.m);
    for (int i = 0; i < m; i++) v[i].copy(mtx.v[i]);
  }
  Matrix(int m, int n) { set(m, n); }
  Matrix(int m, int n, double (*fd)()) {
    set(m, n);
    func(fd);
  }
  Matrix(int m) { set(m); }
  ~Matrix() { _free(); }
  inline double& operator()(const int i, const int j) { return v[i][j]; }
  inline const double& operator()(const int i, const int j) const {
    return v[i][j];
  }
  inline Vector& operator[](const int i) { return v[i]; }
  inline const Vector& operator[](const int i) const { return v[i]; }

  Matrix& operator=(const double& d) {
    for (int i = 0; i < m; i++) v[i] = d;
    return *this;
  }
  Matrix& operator=(const Matrix& mtx) {
    _realloc(mtx.m);
    for (int i = 0; i < m; i++) v[i] = mtx.v[i];
    return *this;
  }

  Vector operator*(Vector& a) {
    Vector vr(m);
    for (int i = 0; i < m; i++) vr[i] = v[i].dot(a);
    return vr;
  }

  Matrix operator+(const Matrix& a) {
    assert(m == a.m);
    Matrix mr(m);
    for (int i = 0; i < m; i++) mr[i] = v[i] + a[i];
    return mr;
  }
  Matrix operator-(const Matrix& a) {
    assert(m == a.m);
    Matrix mr(m);
    for (int i = 0; i < m; i++) mr[i] = v[i] - a[i];
    return mr;
  }

  bool operator==(const Matrix& a) {
    assert(m == a.m);
    for (int i = 0; i < m; i++)
      if (v[i] != a[i]) return false;
    return true;
  }
  bool operator!=(const Matrix& a) {
    assert(m == a.m);
    for (int i = 0; i < m; i++)
      if (v[i] == a[i]) return false;
    return true;
  }

  void func(double (*fd)()) {
    for (int i = 0; i < m; i++) v[i].func(fd);
  }

  void set(int m, int n) {
    _alloc(m);
    for (int i = 0; i < m; i++) v[i].set(n);
  }
  void set(int m) { _alloc(m); }
  void set(const Matrix& mtx) { set(mtx.m, false); }
  void setVect(int i, int n) { v[i].set(n); }

  Matrix& zero() {
    for (auto& vv : *this) vv.zero();
    return *this;
  }
  Matrix& random() {
    for (int i = 0; i < m; i++) v[i].random();
    return *this;
  }
  double sum() {
    double s = 0;
    for (int i = 0; i < m; i++) s += v[i].sum();
    return s;
  }
  Matrix& identity() {
    zero();
    for (int i = 0; i < m; i++) v[i][i] = 1;
    return *this;
  }
  bool isSquared() {
    for (int i = 0; i < m; i++)
      if (m != v[i].size()) return false;
    return true;
  }

  double det() { return _determ(*this, m); }

  Vector* begin() { return &v[0]; }
  Vector* end() { return &v[m]; }

 private:
  double _determ(Matrix v, int m) {
    double det = 0;
    Matrix temp(v.m, v.m);
    int h, k, i, j;

    if (m == 1)
      return v[0][0];
    else if (m == 2)
      return det = (v[0][0] * v[1][1] - v[0][1] * v[1][0]);
    else {
      for (int p = 0; p < m; p++) {
        h = 0;
        k = 0;
        for (i = 1; i < m; i++) {
          for (j = 0; j < m; j++) {
            if (j == p) continue;
            temp[h][k] = v[i][j];
            k++;
            if (k == m - 1) {
              h++;
              k = 0;
            }
          }
        }
        det = det + v[0][p] * pow(-1, p) * _determ(temp, m - 1);
      }
      return det;
    }
  }

  void _alloc(int m) {
    if (m != this->m) {
      this->m = m;
      v = new Vector[m];
      allocate = new bool[1];
      *allocate = true;
    }
  }
  void _free() {
    if (allocate && *allocate) {
      if (v && m) delete[] v;
      v = nullptr;
      *allocate = false;
    }
  }
  void _realloc(int m) {
    if (this->m != m) {
      _free();
      _alloc(m);
    }
  }

 private:
  int m;  // no 'n' -> each vector has it's own dimension
  Vector* v = nullptr;
  bool* allocate = nullptr;
};

class Tensor : public Matrix {
 public:
  Tensor() {}
  Tensor(int nm) { _alloc(nm); }
  Tensor(const Tensor& c) {
    _alloc(c.nm);
    for (int i = 0; i < nm; i++) v[i].set(c[i]);
  }
  ~Tensor() {
    if (v) delete[] v;
    v = nullptr;
  }
  void _alloc(int nm) {
    this->nm = nm;
    v = new Matrix[nm];
  }
  void set(int i, int m, int n) { v[i].set(m, n); }

  inline double& operator()(const int i, const int j, const int k) {
    return v[i](j, k);
  }
  inline const double& operator()(const int i, const int j, const int k) const {
    return v[i](j, k);
  }
  inline Vector& operator()(const int i, const int j) { return v[i][j]; }
  inline const Vector& operator()(const int i, const int j) const {
    return v[i][j];
  }
  inline Matrix& operator[](const int i) { return v[i]; }
  inline const Matrix& operator[](const int i) const { return v[i]; }

 private:
  int nm;
  Matrix* v = nullptr;
};

extern void testVector(), testMatrix();
#endif  // MATRIX_H
