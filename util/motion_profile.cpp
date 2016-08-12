#include "motion_profile.h"
#include <iostream>

#ifdef MOTION_PROFILE_TEST
float speed_distance_calculation(int goal, int current,float k){
	int error;
	float motorv;
	float clipped;
	float max=1;

	error=goal-current;
	motorv=error*k;
	
	if(motorv>max) clipped=max;

	else if(motorv<-max) clipped=-max;

	else clipped = motorv;
	
	return clipped;


}
int main(){
	int g = 100;
	int c = 70;
	float k =.01;
	std::cout << "speed: " << speed_distance_calculation(g,c,k) << std::endl;
	
	return 0;
}

#endif
