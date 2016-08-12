#include "motion_profile.h"
#include <iostream>

#ifdef MOTION_PROFILE_TEST
float speed_distance_calculation(int goal, int current,float k){
	int error = goal-current;
	float motorv = error*k;
	float clipped;
	const float max=1;

	
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
