#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>
#include <iomanip> 

using namespace std;

Panel::Panel():
	in_use(false),
	learn(false),
	cheval(false),
	drawbridge(false),
	shoot_prep(false),
	shoot_low(false),
	collect(false),
	shoot_high(false),
	collector_up(false),
	collector_down(false),
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
	auto_switch(Ten_position_pot{}),
	speed_dial(0)
{}

template<typename T>
bool in_limits(const T a,const T b,const T c){//a is variable to compare, b is the mean, and c is the deviation
	return a >= b-c && a <= b+c;
}

float mid(const float a,const float b){
	return a+(b-a)/2;
}

Switch::Switch():value(0),limits({}){}
Switch::Switch(unsigned int size):value(0),limits(vector<Volt>(size)){}
Switch::Switch(std::vector<Volt> set_limits):value(0),limits(set_limits){}

Ten_position_pot::Ten_position_pot(array<Volt,10> set_limits):value(0),limits(set_limits){}
Ten_position_pot::Ten_position_pot():value(0),limits(TEN_POS_POT_LIMITS){}

void Switch::interpret(const Volt volt){
	const float ROUND=.01;
	float divisor=limits.size();
	float deviation=(1/divisor)+ROUND;
	for(unsigned int i=0; i<limits.size(); i++){
		if(in_limits(volt,limits[i],deviation)){
			value=i;
			return;
		}
	}
	assert(0);
}

unsigned int Switch::get(){
	return value;
}

std::ostream& operator<<(std::ostream& o,Switch a){
	o<<"Switch(";
	o<<"value:"<<a.value;
	o<<" limits:"<<a.limits;
	return o<<")";
}

void Ten_position_pot::interpret(const Volt volt){
	for(unsigned i=0;i<10;i++){
		if(volt<(limits[i] + 0.05)) value= (i!=9) ? (i+1) : 0;
	}
	value=9;
}

unsigned int Ten_position_pot::get(){
	return value;
}

std::ostream& operator<<(std::ostream& o,Ten_position_pot a){
	o<<"Ten_position_pot(";
	o<<"value:"<<a.value;
	o<<" limits:"<<a.limits;
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

Three_position_switch interpret1(const float axis_value){
	static const float BOTTOM=-2,DOWN=-1,MIDDLE=0,UP=1,TOP=2;
	if(axis_value >= mid(BOTTOM,DOWN) && axis_value < mid(DOWN,MIDDLE)) return Three_position_switch::DOWN;
	if(axis_value >= mid(DOWN,MIDDLE) && axis_value < mid(MIDDLE,UP)) return Three_position_switch::MIDDLE;
	if(axis_value >= mid(MIDDLE,UP) && axis_value < mid(UP,TOP)) return Three_position_switch::UP;
	assert(0);
}

Three_position_switch interpret2(const float axis_value){
	static const float DOWN=-.66,MIDDLE=0,UP=.66,DEVIATION=.34;
	if(in_limits(axis_value,DOWN,DEVIATION)) return Three_position_switch::DOWN;
	if(in_limits(axis_value,MIDDLE,DEVIATION)) return Three_position_switch::MIDDLE;
	if(in_limits(axis_value,UP,DEVIATION)) return Three_position_switch::UP;
	assert(0);
}

Three_position_switch interpret3(float axis_value){
	#define AXIS_RANGE(last, curr, next, val) if (axis_value > curr-(curr-last)/2 && axis_value < curr+(next-curr)/2) return val;
	static const float DOWN=-1, MIDDLE=0, UP=1;
	AXIS_RANGE(DOWN, MIDDLE, UP, Three_position_switch::MIDDLE)
	else AXIS_RANGE(MIDDLE, UP, 1.5,Three_position_switch::UP)
	return Three_position_switch::DOWN;
	#undef AXIS_RANGE
}

Two_position_switch interpret_two_pos(const bool value){
	return (value)? Two_position_switch::UP : Two_position_switch::DOWN;
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
	
	p.auto_switch.interpret(d.axis[0]);
	p.auto_mode=auto_mode_convert(p.auto_switch.get());
	
	p.lock_climber = interpret_two_pos(d.button[0]);
	p.tilt_auto = interpret_two_pos(d.button[1]);
	p.sides_auto = interpret_two_pos(d.button[2]);
	p.front_auto = interpret_two_pos(d.button[3]);
	
	{
		#define AXIS_RANGE(axis, last, curr, next, var, val) if (axis > curr-(curr-last)/2 && axis < curr+(next-curr)/2) var = val;
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
		#undef AXIS_RANGE
	}
	p.collector_pos=interpret2(d.axis[5]);
	p.front=interpret2(d.axis[4]);
	p.sides=interpret2(d.axis[6]);
	p.winch=interpret2(d.axis[3]);
	{
		//A three position switch connected to two digital inputs
		p.shooter_mode = Three_position_switch::MIDDLE;
		if (d.button[5]) p.shooter_mode = Three_position_switch::UP;
		if (d.button[6]) p.shooter_mode = Three_position_switch::DOWN;
	}
	p.speed_dial = -d.axis[1];//axis_to_percent(d.axis[1]);
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
	{
		cout<<"Test value   interpret1    interpret2    interpret3      switch(3)\n";
		for(float i=-1; i<=1; i+=.01){
			Switch a({-.66,0,.66});
			a.interpret(i);
			cout<<std::fixed<<std::setprecision(2)<<i<<"         ";
			if(i>=0)cout<<" ";
			cout<<interpret1(i)<<"            "<<interpret2(i)<<"            "<<interpret3(i)<<"            "<<a.get()<<"\n";
		}
		cout<<"\n\n";
	}
	
	Panel p;
	for(unsigned i=0;i<50;i++){
		interpret(driver_station_input_rand());
	}
	cout<<p<<"\n";
	return 0;
}
#endif
