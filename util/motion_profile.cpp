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
void Motion_profile::set_goal(double g){
	goal=g;
}

Motion_profile::Motion_profile():goal(0),vel_modifier(.02){}
Motion_profile::Motion_profile(double g, double m):goal(g),vel_modifier(m){}

#ifdef MOTION_PROFILE_TEST

int main(){
	double dis=0; //inches
	const double target1 = 100*12;//inches

	Motion_profile mp1(target1, .01);	

	for(int i=0;i<=target1;i+=10){//i is distance in inches
		 std::cout << "speedlist " << i << " inches out of "<<target1<<" inches: " << mp1.target_speed(i) << std::endl;
	}
	
	//////
		
	const double target2 = 20*12; //inches
	const double speed = 10*12; //inches per second
	const double VEL_MODIFIER=.01;
	Motion_profile mp2(target2, VEL_MODIFIER);

	const float INCREMENT=.5;//seconds
		
	for(float i=0;i<60;i+=INCREMENT){//i is in seconds
		std::cout << "Time(seconds):" << i << "    Current Distance(inches):" << dis << "    Target Distance(inches):" << target2 << "    "<<mp2.target_speed(dis)<<std::endl;
		if(dis>=(target2-1)){
			std::cout << "Robot has reached the target range" << std::endl;
			break;
		}
		dis += INCREMENT*speed*mp2.target_speed(dis);
	}
	
	return 0;
}

#endif
