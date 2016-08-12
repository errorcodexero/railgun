#include "motion_profile.h"
#include <iostream>
#include <assert.h>

#ifdef MOTION_PROFILE_TEST

template<class T, class Compare>
const T& clamp( const T& v, const T& lo, const T& hi, Compare comp )
{
    return assert( !comp(hi, lo) ),
        comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<class T>
const T& clamp( const T& v, const T& lo, const T& hi )
{
    return clamp( v, lo, hi, std::less<T>() );
}

float speed_distance_calculation(float goal, float current,float k){
	float error = goal-current;
	float motorv = error*k;
	const float max=1;
	
	return clamp(motorv,-max,max);
	
}

int main(){
	int g = 100;//goal distance
	int c = 10;//current distance
	float k =.02;//a=value that is used to modify the speed
	float dis=0; //feet
	const float target = 20; //units in feet
	const int speed = 5; //feet/.5 seconds
	

	std::cout << "speed: " << speed_distance_calculation(g,c,k) << std::endl;

	for(int i=0;i<=100;i+=10) std::cout << "speedlist " << i << ": " << speed_distance_calculation(g,i,k) << std::endl;
	
	for(float i=0;i<60;i+=.5){//i is a unit of seconds
		dis += speed*speed_distance_calculation(target,dis,k);
		std::cout << "Time in seconds: " << i << " Distance in feet: " << dis << " of " << target << std::endl;
		if(dis>=(target-1)){
			std::cout << "Robot has reached the target range" << std::endl;
			break;
		}
	}
	
	return 0;
}

#endif
