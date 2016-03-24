#ifndef NAV2_H
#define NAV2_H

#include<vector>
#include "nav.h"

struct Nav2{
	struct NavS{
		float left; 
		float right;
		float amount;
	};
	struct navloadinput{
		point ptone;
		point pttwo;
		direction dirone;
		direction dirtwo;
	};
	
	struct navinput{
		point navpt;
		direction navdir;
	};
	struct aturn{
		float l;
		float r;
		float dur;
		aturn();
		aturn(float,float,float);
	};

	int stepcounter;
	navloadinput s1;
	navloadinput s2;
	navloadinput s3;
	navloadinput s4;
	navloadinput s5;
	
	aturn Aturn;

	
	unsigned int navindex;
	std::vector<NavS> NavV;
		
	std::vector<NavS> loadnav(navloadinput navin);
	
	Nav2();
};

#endif
