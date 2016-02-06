#ifndef MAZE_h
#define MAZE_H
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//enums
//
//the enums for detremine the orentation 
//////////////////////////////////////////////////////////////////////////////
enum direction { LEFT=0, RIGHT=1, UP=2, DOWN=3};
enum movedir { MFORWARD=0, MRIGHT=1, MLEFT=2, MBACK=3};

struct point{
	int x;
	int y;
};


std::vector<std::pair<int,movedir>> solvemaze(point start,point end,direction startdir,direction enddir);



#endif

