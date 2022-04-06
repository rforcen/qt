cc = g++

sources = common.cpp  johnson.cpp  main.cpp
warns = -Wno-multichar -Wno-subobject-linkage
opts=-std=c++2a -O3
libs=-pthread

poly: $(sources)
	@$(cc) $(opts) $(warns) $(sources) $(libs) -o poly