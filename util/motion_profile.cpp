#include "motion_profile.h"
#include <iostream>
#include <assert.h>

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

double Motion_profile::target_speed(const double current){
	float error = goal-current;
	float motorv = error*vel_modifier;
	const float max=1;
	
	return clamp(motorv,-max,max);
}
void Motion_profile::Set_Goal(double g){
	goal=g;
}

Motion_profile::Motion_profile():goal(0),vel_modifier(.02){}
Motion_profile::Motion_profile(double g, double m):goal(g),vel_modifier(m){}

#ifdef MOTION_PROFILE_TEST

int main(){
	int c = 10;//current distance
	float dis=0; //feet
	const float target = 20; //feet
	const int speed = 10; //feet per second

	Motion_profile mp1(100, .02);	

	std::cout << "speed: " << mp1.target_speed(c) << std::endl;

	for(int i=0;i<=100;i+=10) std::cout << "speedlist " << i << ": " << mp1.target_speed(i) << std::endl;
	
	Motion_profile mp2(target, .02);

	const float INCREMENT=.5;//seconds
		
	for(float i=0;i<60;i+=INCREMENT){//i is in seconds
		std::cout << "Time(seconds):" << i << "    Current Distance(feet):" << dis << "    Target Distance(feet):" << target << "    "<<mp2.target_speed(dis)<<std::endl;
		if(dis>=(target-1)){
			std::cout << "Robot has reached the target range" << std::endl;
			break;
		}
		dis += INCREMENT*speed*mp2.target_speed(dis);
	}
	
	return 0;
}

#endif
