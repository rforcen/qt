
// g++ -std=c++17 pr_johnson.cpp johnson.o -o j
#include "johnson.hpp"

int main()
{
    for (auto j : johnsons)
    {
        auto id = j.first;
        auto fv = j.second;
        auto faces = fv.first;
        auto vertexes = fv.second;
        for (auto v : vertexes)
         for (auto _v:v) printf("%f,",_v);
        // // printf("%ld, ",faces.size());
        // for (auto face : faces)
        // {
        //     // printf(" %ld,", face.size());
        //     // for (auto ixf : face)
        //     //     printf("%d,", ixf);

        //     // printf("[");

        //     for (auto ixf : face)
        //     {
        //         for (auto v3 : vertexes[ixf])
        //             printf("%f,", v3);
        //     }
        //     // printf("],\n");
        // }
        // // printf("\n");
    }
}