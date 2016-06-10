#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>

using namespace std;

Panel::Panel():
	in_use(0),
	learn(0),
	cheval(0),
	drawbridge(0),
	shoot_prep(0),
	shoot_low(0),
	collect(0),
	shoot_high(0),
	collector_up(0),
	collector_down(0),
	lock_climber(Two_position_switch::DOWN),
	tilt_auto(Two_position_switch::DOWN),
	front_auto(Two_position_switch::DOWN),
	sides_auto(Two_position_switch::DOWN),
	collector_pos(Three_position_switch::MIDDLE),
	front(Three_position_switch::MIDDLE),
	sides(Three_position_switch::MIDDLE),
	winch(Three_position_switch::MIDDLE),
	shooter_mode(Three_position_switch::MIDDLE),
	auto_mode(Auto_mode::NOTHING),
	auto_switch(0),
	speed_dial(0)
{}

Ten_position_pot::Ten_position_pot(array<Volt,10> set_limits):value(0),limits(set_limits){}
Ten_position_pot::Ten_position_pot(Volt v):value(v),limits(TEN_POS_POT_LIMITS){}
Ten_position_pot::Ten_position_pot():value(0),limits(TEN_POS_POT_LIMITS){}

void Ten_position_pot::interpret(const Volt volt){
	for(unsigned i=0;i<10;i++){
		if(volt<(limits[i] + 0.05)) value= (i!=9) ? (i+1) : 0;
	}
	value=9;
}

std::array<Volt,10> Ten_position_pot::get_limits(){
	return limits;
}

unsigned int Ten_position_pot::get(){
	return value;
}

std::ostream& operator<<(std::ostream& o,Ten_position_pot a){
	o<<"Ten_position_pot(";
	o<<"value:"<<a.get();
	o<<" limits:"<<a.get_limits();
	return o<<")";
}

std::ostream& operator<<(std::ostream& o,Three_position_switch a){
	#define X(NAME) if(a==Three_position_switch::NAME)return o<<""#NAME;
	X(UP) X(MIDDLE) X(DOWN)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Two_position_switch a){
	#define X(NAME) if(a==Two_position_switch::NAME)return o<<""#NAME;
	X(UP) X(DOWN)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Panel::Auto_mode a){
	o<<"Panel::Auto_mode(";
	#define X(name) if(a==Panel::Auto_mode::name)o<<""#name;
	X(NOTHING) X(REACH) X(STATICS) X(STATICF) X(PORTCULLIS) X(CHEVAL) X(LBLS) X(LBWLS) X(LBWHS) X(S)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Panel p){
	o<<"Panel(";
	o<<"in_use:"<<p.in_use;
	#define X(name) o<<", "#name":"<<p.name;
	X(learn) X(cheval) X(drawbridge) X(shoot_prep) X(shoot_low) X(collect) X(shoot_high) X(collector_up) X(collector_down) //buttons
	X(lock_climber) X(tilt_auto) X(front_auto) X(sides_auto) //2-pos switches
	X(collector_pos) X(front) X(sides) X(winch) X(shooter_mode) //3-pos switches
	X(auto_mode) //10-pos switches
	o<<", auto_switch:"<<p.auto_switch.get();
	X(speed_dial) //Dials
	#undef X
	return o<<")";
}

Panel::Auto_mode auto_mode_convert(int potin){
	switch(potin) {
		case 0:
			return Panel::Auto_mode::NOTHING;
		case 1:
			return Panel::Auto_mode::REACH;
		case 2:
			return Panel::Auto_mode::STATICS;
		case 3:
			return Panel::Auto_mode::STATICF;
		case 4:
			return Panel::Auto_mode::PORTCULLIS;
		case 5: 
			return Panel::Auto_mode::CHEVAL;
		case 6:
			return Panel::Auto_mode::LBLS;
		case 7:
			return Panel::Auto_mode::LBWLS;
		case 8:
			return Panel::Auto_mode::LBWHS;	
		case 9: 
			return Panel::Auto_mode::S;
		default:
			return Panel::Auto_mode::NOTHING;
	}
}

float axis_to_percent(double a){
	return .5-(a/2);
}

Three_position_switch interpret(float axis_value){
	static const float DOWN=0,UP=-.5,MIDDLE=-1;;
	if(axis_value>UP-(UP-MIDDLE)/2 && axis_value<UP+(DOWN-UP)/2)return Three_position_switch::UP;
	if(axis_value>DOWN-(DOWN-UP)/2 && axis_value<DOWN+.25)return Three_position_switch::DOWN;
	return Three_position_switch::MIDDLE;
}

Panel interpret(Joystick_data d){
	Panel p;
	{
		p.in_use=[&](){
			for(int i=0;i<JOY_AXES;i++) {
				if(d.axis[i]!=0)return true;
			}
			for(int i=0;i<JOY_BUTTONS;i++) {
				if(d.button[i]!=0)return true;
			}
			return false;
		}();
		if (!p.in_use) return p;
	}
	{
		Volt auto_mode=d.axis[0];
		p.auto_switch.interpret(auto_mode);
		p.auto_mode=auto_mode_convert(p.auto_switch.get());
	}
	p.lock_climber = (d.button[0])? Two_position_switch::UP : Two_position_switch::DOWN;
	p.tilt_auto = (d.button[1])? Two_position_switch::UP : Two_position_switch::DOWN;
	p.sides_auto = (d.button[2])? Two_position_switch::UP : Two_position_switch::DOWN;
	p.front_auto = (d.button[3])? Two_position_switch::UP : Two_position_switch::DOWN;
	#define AXIS_RANGE(axis, last, curr, next, var, val) if (axis > curr-(curr-last)/2 && axis < curr+(next-curr)/2) var = val;
	{
		float op = d.axis[2];
		static const float DEFAULT=-1, COLLECTOR_UP=-.8, COLLECTOR_DOWN=-.62, SHOOT_HIGH=-.45, COLLECT=-.29, SHOOT_LOW=-.11, SHOOT_PREP=.09, DRAWBRIDGE=.33, CHEVAL=.62, LEARN=1;
		#define X(button) p.button = 0;
		X(collector_up) X(collector_down) X(shoot_high) X(collect) X(shoot_low) X(shoot_prep) X(drawbridge) X(cheval) X(learn)
		#undef X
		AXIS_RANGE(op, DEFAULT, COLLECTOR_UP, COLLECTOR_DOWN, p.collector_up, 1)
		else AXIS_RANGE(op, COLLECTOR_UP, COLLECTOR_DOWN, SHOOT_HIGH, p.collector_down, 1)
		else AXIS_RANGE(op, COLLECTOR_DOWN, SHOOT_HIGH, COLLECT, p.shoot_high, 1)
		else AXIS_RANGE(op, SHOOT_HIGH, COLLECT, SHOOT_LOW, p.collect, 1)
		else AXIS_RANGE(op, COLLECT, SHOOT_LOW, SHOOT_PREP, p.shoot_low, 1)
		else AXIS_RANGE(op, SHOOT_LOW, SHOOT_PREP, DRAWBRIDGE, p.shoot_prep, 1)
		else AXIS_RANGE(op, SHOOT_PREP, DRAWBRIDGE, CHEVAL, p.drawbridge, 1)
		else AXIS_RANGE(op, DRAWBRIDGE, CHEVAL, LEARN, p.cheval, 1)
		else AXIS_RANGE(op, CHEVAL, LEARN, 1.38, p.learn, 1)
	}
	p.collector_pos=interpret(d.axis[5]);
	p.front=interpret(d.axis[4]);
	p.sides=interpret(d.axis[6]);
	p.winch=interpret(d.axis[3]);
	{
		//A three position switch connected to two digital inputs
		p.shooter_mode = Three_position_switch::MIDDLE;
		if (d.button[5]) p.shooter_mode = Three_position_switch::UP;
		if (d.button[6]) p.shooter_mode = Three_position_switch::DOWN;
	}
	p.speed_dial = -d.axis[1];//axis_to_percent(d.axis[1]);
	#undef AXIS_RANGE
	return p;
}

#ifdef PANEL_TEST
Joystick_data driver_station_input_rand(){
	Joystick_data r;
	for(unsigned i=0;i<JOY_AXES;i++){
		r.axis[i]=(0.0+rand()%101)/100;
	}
	for(unsigned i=0;i<JOY_BUTTONS;i++){
		r.button[i]=rand()%2;
	}
	return r;
}

int main(){
	Panel p;
	for(unsigned i=0;i<50;i++){
		interpret(driver_station_input_rand());
	}
	cout<<p<<"\n";
	return 0;
}
#endif
