#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>

using namespace std;

Panel::Panel():
	in_use(0),
	control_tilt(0),
	eject(0),
	collect(0),
	reflect(0),
	stow(0),
	open_portcullis(0),
	lower_cheval(0),
	collector_auto(0),
	climber(Climber::STOP),
	front(Collector::OFF),
	sides(Collector::OFF),
	tilt(Tilt::STOP),
	auto_mode(Auto_mode::NOTHING),
	angle(0)
{}

ostream& operator<<(ostream& o,Panel::Climber a){
	o<<"Panel::Climber(";
	#define X(name) if(a==Panel::Climber::name)o<<""#name;
	X(EXTEND) X(STOP) X(RETRACT)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Panel::Collector a){
	o<<"Panel::Collector(";
	#define X(name) if(a==Panel::Collector::name)o<<""#name;
	X(IN) X(OUT) X(OFF)	
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Panel::Tilt a){
	o<<"Panel::Tilt(";
	#define X(name) if(a==Panel::Tilt::name)o<<""#name;
	X(UP) X(DOWN) X(STOP)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o, Panel::Auto_mode a){
	o<<"Panel::Auto_mode(";
	#define X(name) if(a==Panel::Auto_mode::name)o<<""#name;
	X(NOTHING) X(MOVE) X(SHOOT)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Panel p){
	o<<"Panel(";
	o<<"in_use:"<<p.in_use;
	o<<", control_tilt:"<<p.control_tilt;
	o<<", eject:"<<p.eject;
	o<<", collect:"<<p.collect;
	o<<", reflect:"<<p.reflect;
	o<<", stow:"<<p.stow;
	o<<", open_portcullis:"<<p.open_portcullis;
	o<<", lower_cheval:"<<p.lower_cheval;
	o<<", collector_auto:"<<p.collector_auto;
	o<<", climber:"<<p.climber;
	o<<", front:"<<p.front;
	o<<", sides:"<<p.sides;
	o<<", tilt:"<<p.tilt;
	o<<", auto_mode:"<<p.auto_mode;
	o<<", angle:"<<p.angle;
	return o<<")";
}

Panel::Auto_mode auto_mode_convert(int potin){
	switch(potin) {
		case 1:
			return Panel::Auto_mode::MOVE;
		case 2:
			return Panel::Auto_mode::SHOOT;
		default:
			return Panel::Auto_mode::NOTHING;
	}
}

Panel interpret(Joystick_data d){
	#define AXIS_RANGE(axis, last, curr, next, button) if (axis > curr-(curr-last)/2 && axis < curr+(next-curr)) button = 1;
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
	}
	{
		Volt auto_mode=d.axis[0];
		p.auto_mode=auto_mode_convert(interpret_10_turn_pot(auto_mode));
	}
	{
		float op=d.axis[1];
		static const float DEFAULT=-1, COLLECT=-.7, REFLECT=-.4, EJECT=0, STOW=.4, PORTCULLIS=.7, CHEVAL=1;
		#define X(button) p.button = 0;
		X(collect) X(reflect) X(eject) X(stow) X(open_portcullis) X(lower_cheval)
		#undef X
		AXIS_RANGE(op, DEFAULT, COLLECT, REFLECT, p.collect)
		else AXIS_RANGE(op, COLLECT, REFLECT, EJECT, p.reflect)
		else AXIS_RANGE(op, REFLECT, EJECT, STOW, p.eject)
		else AXIS_RANGE(op, EJECT, STOW, PORTCULLIS, p.stow)
		else AXIS_RANGE(op, STOW, PORTCULLIS, CHEVAL, p.open_portcullis)
		else AXIS_RANGE(op, PORTCULLIS, CHEVAL, 1.5, p.lower_cheval)
	}
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
