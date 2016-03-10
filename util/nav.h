#ifndef NAV_H
#define NAV_H

#include <vector>
#define ROBOTSPACE (18/2)
///////////////////////////////////////////////////////////////////////////////
//enums
//
//the enums for detremine the orentation 
//////////////////////////////////////////////////////////////////////////////
enum direction { LEFT=0, RIGHT=1, UP=2, DOWN=3};
enum movedir { MFORWARD=0, MRIGHT=1, MLEFT=2, MBACK=3};

struct point{
	int x, y;
};

std::vector<std::pair<int,movedir>> solvemaze(point,point,direction,direction);

#endif

