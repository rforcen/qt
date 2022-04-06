#ifndef SIMD_H
#define SIMD_H

#include <math.h>

namespace simdfloat3 {

class Simd {
 public:
  typedef float simd_f3 __attribute__((ext_vector_type(3)));
  typedef int v4si __attribute__((vector_size(16)));

 public:
  simd_f3 v;

 public:
  inline Simd() { v = 0; }
  inline Simd(simd_f3 v) : v(v) {}
  inline Simd(Simd &v) : v(v.v) {}
  inline Simd(float f) : v(f) {}

  inline Simd operator+(Simd vo) { return v + vo.v; }
  inline Simd operator-(Simd vo) { return v - vo.v; }
  inline Simd operator*(Simd vo) { return v * vo.v; }
  inline Simd operator/(Simd vo) { return v / vo.v; }

  inline Simd operator+=(Simd vo) {
    v += vo.v;
    return *this;
  }
  inline Simd operator-=(Simd vo) {
    v -= vo.v;
    return *this;
  }
  inline Simd operator*=(Simd vo) {
    v *= vo.v;
    return *this;
  }
  inline Simd operator/=(Simd vo) {
    v += vo.v;
    return *this;
  }

  inline float len_sq() { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; }
  inline float len() { return sqrtf(len_sq()); }
  inline float sum() { return v[0] + v[1] + v[2]; }
  inline Simd unit() { return v / len(); }
};

inline Simd mid(Simd v1, Simd v2) { return (v1 + v2) / 2; }
inline float len_sq(Simd v) { return v.len_sq(); }
inline float len(Simd v) { return v.len(); }
inline float dot(Simd v1, Simd v2) { return (v1 * v2).sum(); }
inline static Simd unit(Simd v) { return v.unit(); }
inline Simd cross(Simd v1, Simd v2) {
  return (v1.v.zxy * v2.v - v1.v * v2.v.zxy)[2].xy;
}

}  // namespace simdfloat3
#endif  // SIMD_H
