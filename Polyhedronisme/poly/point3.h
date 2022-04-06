// point3

#pragma once

#include <initializer_list>
#include <math.h>
#include <stdio.h>

class point3 {
public:
  float x = 0, y = 0, z = 0;

  inline point3() {} // constructors
  inline point3(float x, float y, float z) : x(x), y(y), z(z) {}
  inline point3(float f) : x(f), y(f), z(f) {}
  inline point3(const point3 &o) : x(o.x), y(o.y), z(o.z) {}

  point3(std::initializer_list<float> l) {
    int i = 0;
    for (auto ll : l) {
      switch (i) {
      case 0:
        x = ll;
        break;
      case 1:
        y = ll;
        break;
      case 2:
        z = ll;
        break;
      }
      i++;
    }
  }

  inline static point3 zero() { return point3(0); }
  inline static point3 one() { return point3(1); }

  inline point3 unit() { return *this / distSq(*this); }
  inline void normalize() { *this /= dist(*this); } // insite

  inline point3 mid(point3 &o) { return (*this + o) / 2; }

  inline static float max(float x, float y) {
    return fabs(x) > fabs(y) ? fabs(x) : fabs(y);
  }
  inline float max_abs() { return max(max(x, y), z); }

  inline static point3 tween(point3 v1, point3 v2, float t) {
    return (v1 * (1 - t)) + (v2 * t);
  }
  inline static point3 oneThird(point3 v1, point3 v2) {
    return tween(v1, v2, 1.f / 3.f);
  }

  inline point3 tween(point3 &o, float t) {
    return (*this * (1 - t)) + (o * t);
  }
  inline point3 oneThird(point3 &o) { return tween(o, 1.f / 3.f); }

  inline void operator=(point3 o) {
    x = o.x;
    y = o.y;
    z = o.z;
  }
  inline bool operator==(float f) { return x == f && y == f && z == f; }
  inline bool operator!=(float f) { return x != f || y != f || z != f; }

  inline bool operator==(point3 &o) { return x == o.x && y == o.y && z == o.z; }
  inline bool operator!=(point3 &o) { return x != o.x || y != o.y || z != o.z; }

  inline point3 operator+(point3 o) {
    return point3(x + o.x, y + o.y, z + o.z);
  }
  inline point3 operator-(point3 o) {
    return point3(x - o.x, y - o.y, z - o.z);
  }
  inline point3 operator*(point3 o) {
    return point3(x * o.x, y * o.y, z * o.z);
  }
  inline point3 operator/(point3 o) {
    return point3(x / o.x, y / o.y, z / o.z);
  }

  inline point3 operator+(float f) { return point3(x + f, y + f, z + f); }
  inline point3 operator-(float f) { return point3(x - f, y - f, z - f); }
  inline point3 operator*(float f) { return point3(x * f, y * f, z * f); }
  inline point3 operator/(float f) { return point3(x / f, y / f, z / f); }

  inline point3 operator-() { return point3(-x, -y, -z); }
  inline point3 operator+() { return *this; }

  inline void operator+=(float f) { x += f, y += f, z += f; }
  inline void operator-=(float f) { x -= f, y -= f, z -= f; }
  inline void operator+=(point3 o) { x += o.x, y += o.y, z += o.z; }
  inline void operator-=(point3 o) { x -= o.x, y -= o.y, z -= o.z; }
  inline void operator*=(float f) { x *= f, y *= f, z *= f; }
  inline void operator/=(float f) { x /= f, y /= f, z /= f; }
  inline void operator*=(point3 o) { x *= o.x, y *= o.y, z *= o.z; }
  inline void operator/=(point3 o) { x /= o.x, y /= o.y, z /= o.z; }

  inline float distanceSq(point3 &o) { return dot(o); }
  inline float distSq(point3 &o) { return dot(o); }
  inline float dist(point3 &o) { return sqrtf(dot(o)); }

  inline float dot(point3 &o) { return x * o.x + y * o.y + z * o.z; }
  // inline float dot(point3 o) { return x * o.x + y * o.y + z * o.z; }
  inline static float dot(point3 &p0, point3 &p1) {
    return p0.x * p1.x + p0.y * p1.y + p0.z * p1.z;
  }

  inline static point3 cross(point3 &v0, point3 &v1) {
    return point3((v0.y * v1.z) - (v0.z * v1.y), (v0.z * v1.x) - (v0.x * v1.z),
                  (v0.x * v1.y) - (v0.y * v1.x));
  }
  inline static point3 cross(point3 v0, point3 v1) {
    return point3((v0.y * v1.z) - (v0.z * v1.y), (v0.z * v1.x) - (v0.x * v1.z),
                  (v0.x * v1.y) - (v0.y * v1.x));
  }
  inline point3 cross(point3 &v1) {
    return point3((y * v1.z) - (z * v1.y), (z * v1.x) - (x * v1.z),
                  (x * v1.y) - (y * v1.x));
  }
  inline static point3 normal(point3 &v0, point3 &v1, point3 &v2) {
    return cross(v1 - v0, v2 - v1);
  }

  void print(const char *lbl = "", const char *end = "") {
    printf("%s%f %f %f%s\n", lbl, x, y, z, end);
  }
};

static void test_point3() {
  point3 p0(1), p1(2), p2(1, 2, 3);

  p0 += p1;
  p1 += 333;

  p0.print("p0=");
  p1.print("p1=");
  p2.print("p2=");

  point3 dots(p0.dot(p1), p1.dot(p2), p2.dot(p0));
  dots.print("dots=");

  p0.print("p0=");
  p1.print("p1=");

  point3 cr;
  cr = p2.cross(p1);
  cr.print("cr=");
  cr = -cr;
  cr.print("cr=");

  p0 = point3(13);
  p1 = point3(23);
  p0.print("p0=");
  p1.print("p1=");
  p1 = point3(1, 2, 3);
  cr = p0.cross(p1);
  cr.print("cr=");

  p0 = point3(4, 52, 6);
  p2 = point3(72, 8, 9);
  point3 n = point3::normal(p0, p1, p2);
  n.print("normal=");

  p0.print("p0=");
  p1.print("p1=");
  auto p3 = p0.oneThird(p1);
  p3.print("1/3=");
  p3.normalize();
  p3.print("1/3 normalize=");

  puts("");
}
