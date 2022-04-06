//
//  johnson.hpp
//  test_polygon
//
//  Created by asd on 04/09/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef johnson_hpp
#define johnson_hpp

#include "common.hpp"
#include <stdlib.h>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>


using std::vector, std::string, std::map, std::pair, std::tuple;


typedef pair<Faces, Vertexes> FacesVertexes;
typedef map<int, FacesVertexes> Johnsons;

extern Johnsons johnsons;

#endif /* johnson_hpp */
