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
	lock_climber(2),
	tilt_auto(2),
	front_auto(2),
	sides_auto(2),
	collector_pos(3),
	front(3),
	sides(3),
	winch(3),
	shooter_mode(3),
	auto_mode(Auto_mode::NOTHING),
	auto_switch(TEN_POS_POT_LIMITS),
	speed_dial(0.0)
{}

float mid(const float a,const float b){
	return a+(b-a)/2;
}

Switch::Switch():value(0),targets({}){}
Switch::Switch(unsigned int size):value(0),targets({}){
	assert(size>1);
	for(unsigned int i=0; i<size; i++){
		targets.push_back(-1+((1+2*i)/(float)size));
	}
}
Switch::Switch(std::vector<Volt> set_targets):value(0),targets({}){
	assert(set_targets.size()>1);
	targets=set_targets;
}

void Switch::interpret(const Volt volt){
	vector<Volt> temp=targets;
	temp.insert(temp.begin(),-2.0);
	temp.insert(temp.end(),2.0);
	for(unsigned int i=1; i<temp.size()-1; i++){
		if(volt >= mid(temp[i-1],temp[i]) && volt < mid(temp[i],temp[i+1])){
			value=i-1;
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
	o<<" targets:"<<a.targets;
	return o<<")";
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
	
	p.lock_climber.interpret(d.button[0]);
	p.tilt_auto.interpret(d.button[1]);
	p.sides_auto.interpret(d.button[2]);
	p.front_auto.interpret(d.button[3]);
	
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
	p.collector_pos.interpret(d.axis[5]);
	p.front.interpret(d.axis[4]);
	p.sides.interpret(d.axis[6]);
	p.winch.interpret(d.axis[3]);
	{
		//A three position switch connected to two digital inputs
		//p.shooter_mode = Three_position_switch::MIDDLE;//TODO Come back to this
		//if (d.button[5]) p.shooter_mode = Three_position_switch::UP;
		//if (d.button[6]) p.shooter_mode = Three_position_switch::DOWN;
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
		cout<<"Test value   switch(3)\n";
		for(float i=-1; i<=1; i+=.01){
			Switch a(3);
			a.interpret(i);
			cout<<std::fixed<<std::setprecision(2)<<i<<"         ";
			if(i>=0)cout<<" ";
			cout<<a.get()<<"\n";
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
