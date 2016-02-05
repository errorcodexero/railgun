#ifndef MAZE_h
#define MAZE_H
#include <vector>
#define MAPWIDTH 160
#define MAPLENGTH 162//add.5

struct mapstruct {
	int width;
	int length;
	bool walls[MAPWIDTH][MAPLENGTH];
};
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

