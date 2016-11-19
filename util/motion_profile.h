#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H

struct Motion_profile{
	double goal;//inches
	double vel_modifier;
	
	double target_speed(const double);
	void set_goal(double);

	Motion_profile();
	Motion_profile(double,double);	
	
};

#endif
