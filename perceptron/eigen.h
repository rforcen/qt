#ifndef EIGEN_H
#define EIGEN_H

// eigen wrapper

#define EIGEN_NO_DEBUG

#include <stdlib.h>
#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <vector>

using Eigen::VectorXd;
using Eigen::MatrixXd;
using Eigen::Dynamic;
using Eigen::Matrix;
using Eigen::VectorXi;
using Eigen::MatrixBase;
using Eigen::Vector3d;

typedef VectorXd vec;
typedef MatrixXd mat;
typedef Matrix<mat, Dynamic, 1> mdMatrix;  // a vector of matrix
typedef Matrix<vec, Dynamic, 1> mdVector;  // a vector of vectors

typedef Matrix<mdVector, Dynamic, 1> mmdVector;  // a vector of vectors

using std::cout;
using std::endl;

using std::ptr_fun;
using std::vector;
using std::pair;
using std::make_pair;

VectorXi rndRange(int n);

typedef VectorXd vec;
typedef Matrix<vec, Dynamic, 1> vvec;  // a vector of vectors

class Vect : public vvec {
 public:
  Vect(void) : vvec() {}
  Vect(int nRows) : vvec(nRows) {}

  template <typename T>
  Vect(const MatrixBase<T>& other) : Vector3d(other) {}

  template <typename T>
  Vect& operator=(const MatrixBase<T>& other) {
    vvec::operator=(other);
    return *this;
  }
  Vect& operator=(const double d) {
    for (int r = 0; r < rows(); r++) self(r).fill(d);
    return *this;
  }

  Vect operator*(const double d) {
    Vect res = shape(*this);
    for (int r = 0; r < rows(); r++) res[r] = (*this)[r] * d;
    return res;
  }
  Vect operator*(const Vect& v) {
    Vect res = shape(*this);
    for (int r = 0; r < rows(); r++) res[r] = self(r).cwiseProduct(v[r]);
    return res;
  }
  Vect& operator+=(const Vect& v) {
    for (int r = 0; r < rows(); r++) self(r) += v[r];
    return self();
  }
  Vect& operator-=(const Vect& v) {
    for (int r = 0; r < rows(); r++) self(r) -= v[r];
    return self();
  }
  inline vec& operator[](const Index ix) {
    return vvec::operator[](ix >= 0 ? ix : rows() + ix);
  }
  inline const vec& operator[](const Index ix) const {
    return vvec::operator[](ix >= 0 ? ix : rows() + ix);
  }
  void zero() {
    for (int r = 0; r < rows(); r++) (*this)[r].setZero();
  }
  void random() {
    for (int r = 0; r < rows(); r++) (*this)[r].setRandom();
  }
  Vect shape(Vect& v) {
    Vect vs(v.rows());
    for (int r = 0; r < v.rows(); r++) vs[r] = vec(v[r].rows());
    return vs;
  }
  Vect shape() {
    Vect vs(rows());
    for (int r = 0; r < rows(); r++) vs[r] = vec(self(r).rows());
    return vs;
  }
  inline int rows() const { return this->vvec::rows(); }
  inline vec& self(int ix) { return (*this)[ix]; }
  inline Vect& self() { return *this; }
};

void testVect();

#endif  // EIGEN_H
