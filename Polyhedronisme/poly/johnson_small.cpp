//
//  johnson_small.cpp

#include "johnson.hpp"

Johnsons johnsons = {
    {1,
     {{{1, 4, 2}, {0, 1, 2}, {3, 0, 2}, {4, 3, 2}, {4, 1, 0, 3}},
      {{-0.729665f, 0.670121f, 0.319155f},
       {-0.655235f, -0.29213f, -0.754096f},
       {-0.093922f, -0.607123f, 0.537818f},
       {0.702196f, 0.595691f, 0.485187f},
       {0.776626f, -0.36656f, -0.588064f}}}},
    {2,
     {{{3, 0, 2}, {5, 3, 2}, {4, 5, 2}, {1, 4, 2}, {0, 1, 2}, {0, 3, 5, 4, 1}},
      {{-0.868849f, -0.100041f, 0.61257f},
       {-0.329458f, 0.976099f, 0.28078f},
       {-0.26629f, -0.013796f, -0.477654f},
       {-0.13392f, -1.034115f, 0.229829f},
       {0.738834f, 0.707117f, -0.307018f},
       {0.859683f, -0.535264f, -0.338508f}}}},
};

// #define EXPORT_NIM

#ifdef EXPORT_NIM
int main()
{
  puts("[");
  for (auto j : johnsons)
  {
    printf("Johnsons(i:%d, \nfaces: @[", j.first);
    for (auto face : j.second.first)
    { // faces
      printf("@[");
      for (auto ix : face)
        printf("%d,", ix);
      printf("],");
    }
    printf("], \nvertex: @["); // vertex
    for (auto vertex : j.second.second)
    {
      printf("[");
      for (auto v : vertex)
        printf("%f,", v);
      printf("], ");
    }
    puts("]),");
  }
  puts("]");
  return 0;
}
#endif