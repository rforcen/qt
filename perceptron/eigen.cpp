#include <eigen.h>

VectorXi rndRange(int n) {
  VectorXi indices = VectorXi::LinSpaced(n, 0, n);
  std::random_shuffle(indices.data(), indices.data() + n);
  return indices;
}

void testVect() {
  Vect v1, v2, v3;

  v1 = Vect(10);
  for (int i = 0; i < 10; i++) v1[i] = vec(10);
  v1.random();

  auto lv1 = v1[-1];  // python like index : neg index == rows()+index

  v2 = v1 * 100;
  v3 = v1.shape();

  v3 = v1 * v2;

  v1 = 1, v2 = 2;

  v2 += v1;

  v1.zero();
  v3 = v1;
}
