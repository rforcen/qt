#pragma once

#include <math.h>

namespace simdfloat3 {

class Simd {
public:
  //  typedef float simd_f3 __attribute__((ext_vector_type(3)));
  //  typedef int v4si __attribute__((vector_size(16)));
  class simd_f3 {
  public:
    simd_f3() {}
    simd_f3(float x, float y, float z) : x(x), y(y), z(z) {}
    simd_f3(float &f) : x(f), y(f), z(f) {}
    void operator=(float f) {
      x = f;
      y = f;
      z = f;
    }
    inline simd_f3 operator+(simd_f3 &o) {
      return simd_f3(x + o.x, y + o.y, z + o.z);
    }
    inline simd_f3 operator-(simd_f3 &o) {
      return simd_f3(x - o.x, y - o.y, z - o.z);
    }
    inline simd_f3 operator*(simd_f3 &o) {
      return simd_f3(x * o.x, y * o.y, z * o.z);
    }
    inline simd_f3 operator/(simd_f3 &o) {
      return simd_f3(x / o.x, y / o.y, z / o.z);
    }
    inline simd_f3 operator/(float f) { return simd_f3(x / f, y / f, z / f); }
    inline simd_f3 operator+=(simd_f3 &o) {
      *this = *this + o;
      return *this;
    }
    inline simd_f3 operator-=(simd_f3 &o) {
      *this = *this - o;
      return *this;
    }
    inline simd_f3 operator*=(simd_f3 &o) {
      *this = *this * o;
      return *this;
    }
    inline simd_f3 operator/=(simd_f3 &o) {
      *this = *this / o;
      return *this;
    }
    float x = 0, y = 0, z = 0;
  };

public:
  simd_f3 v;

public:
  inline Simd() { v = 0; }
  inline Simd(simd_f3 v) : v(v) {}
  inline Simd(Simd &v) : v(v.v) {}
  inline Simd(float &f) : v(f) {}
  inline Simd(float x, float y, float z) : v(x, y, z) {}

  inline Simd operator+(Simd &vo) { return v + vo.v; }
  inline Simd operator-(Simd &vo) { return v - vo.v; }
  inline Simd operator*(Simd &vo) { return v * vo.v; }
  inline Simd operator/(Simd &vo) { return v / vo.v; }
  inline Simd operator/(float f) { return v / f; }

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

  inline float len_sq() { return v.x * v.x + v.y * v.y + v.z * v.z; }
  inline float len() { return sqrtf(len_sq()); }
  inline float sum() { return v.x + v.y + v.z; }
  inline Simd unit() { return v / len(); }
};

inline Simd mid(Simd v1, Simd v2) { return (v1 + v2) / 2.0f; }
inline float len_sq(Simd v) { return v.len_sq(); }
inline float len(Simd v) { return v.len(); }
inline float dot(Simd v1, Simd v2) { return (v1 * v2).sum(); }
inline static Simd unit(Simd v) { return v.unit(); }
inline Simd cross(Simd a, Simd b) {
  return Simd{(a.v.y * b.v.z) - (a.v.z * b.v.y),
              (a.v.z * b.v.x) - (a.v.x * b.v.z),
              (a.v.x * b.v.y) - (a.v.y * b.v.x)};
}

} // namespace simdfloat3
