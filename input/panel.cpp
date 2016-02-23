#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>

using namespace std;

Panel::Panel():
	in_use(0),
	control_angle(0),
	eject(0),
	collect(0),
	reflect(0),
	stow(0),
	terrain(0),
	low_bar(0),
	prep_cheval(0),
	prep_drawbridge(0),
	prep_sally(0),
	prep_portcullis(0),
	execute_prep(0),
	cancel_prep(0),
	prep_shoot(0),
	shoot(0),
	collector_auto(0),
	climber(Climber::STOP),
	front(Collector::OFF),
	sides(Collector::OFF),
	tilt(Tilt::STOP),
	winch(Winch::STOP),
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


ostream& operator<<(ostream& o,Panel::Winch a){
        o<<"Panel::Winch(";
        #define X(name) if(a==Panel::Winch::name)o<<""#name;
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
	o<<", control_angle:"<<p.control_angle;
	o<<", eject:"<<p.eject;
	o<<", collect:"<<p.collect;
	o<<", reflect:"<<p.reflect;
	o<<", stow:"<<p.stow;
	o<<", terrain:"<<p.terrain;
	o<<", low_bar:"<<p.low_bar;
	o<<", prep_cheval:"<<p.prep_cheval;
	o<<", prep_drawbridge:"<<p.prep_drawbridge;
	o<<", prep_sally:"<<p.prep_sally;
	o<<", prep_portcullis:"<<p.prep_portcullis;
	o<<", execute_prep:"<<p.execute_prep;
	o<<", cancel_prep:"<<p.cancel_prep;
	o<<", prep_shoot:"<<p.prep_shoot;
	o<<", shoot:"<<p.shoot;
	o<<", collector_auto:"<<p.collector_auto;
	o<<", climber:"<<p.climber;
	o<<", front:"<<p.front;
	o<<", sides:"<<p.sides;
	o<<", tilt:"<<p.tilt;
	o<<", winch:"<<p.winch;
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
	#define AXIS_RANGE(axis, last, curr, next, var, val) if (axis > curr-(curr-last)/2 && axis < curr+(next-curr)/2) var = val;
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
		Volt auto_mode=d.axis[1];
		p.auto_mode=auto_mode_convert(interpret_10_turn_pot(auto_mode));
	}
	p.collector_auto = d.button[10];
	p.control_angle = d.button[0];
	{
		float op = d.axis[3];
		static const float DEFAULT=-1, COLLECT=-.87, REFLECT=-.76, EJECT=-.66, STOW=-.56, TERRAIN=-.47, LOW_BAR=-.37, CHEVAL=-.27, DRAWBRIDGE=-.16, SALLY=-.04, PORTCULLIS=.1, EXECUTE=.26, CANCEL=.45, PREP_SHOOT=.69, SHOOT=1;
		#define X(button) p.button = 0;
		X(collect) X(reflect) X(eject) X(stow) X(terrain) X(low_bar) X(prep_cheval) X(prep_drawbridge) X(prep_sally) X(prep_portcullis) X(execute_prep) X(cancel_prep) X(prep_shoot) X(shoot)
		#undef X
		AXIS_RANGE(op, DEFAULT, COLLECT, REFLECT, p.collect, 1)
		else AXIS_RANGE(op, COLLECT, REFLECT, EJECT, p.reflect, 1)
		else AXIS_RANGE(op, REFLECT, EJECT, STOW, p.eject, 1)
		else AXIS_RANGE(op, EJECT, STOW, TERRAIN, p.stow, 1)
		else AXIS_RANGE(op, STOW, TERRAIN, LOW_BAR, p.terrain, 1)
		else AXIS_RANGE(op, TERRAIN, LOW_BAR, CHEVAL, p.low_bar, 1)
		else AXIS_RANGE(op, LOW_BAR, CHEVAL, DRAWBRIDGE, p.prep_cheval, 1)
		else AXIS_RANGE(op, CHEVAL, DRAWBRIDGE, SALLY, p.prep_drawbridge, 1)
		else AXIS_RANGE(op, DRAWBRIDGE, SALLY, PORTCULLIS, p.prep_sally, 1)
		else AXIS_RANGE(op, SALLY, PORTCULLIS, EXECUTE, p.prep_portcullis, 1)
		else AXIS_RANGE(op, PORTCULLIS, EXECUTE, CANCEL, p.execute_prep, 1)
		else AXIS_RANGE(op, EXECUTE, CANCEL, PREP_SHOOT, p.cancel_prep, 1)
		else AXIS_RANGE(op, CANCEL, PREP_SHOOT, SHOOT, p.prep_shoot, 1)
		else AXIS_RANGE(op, PREP_SHOOT, SHOOT, 1.28, p.shoot, 1)
	}
	{
		float climb = d.axis[2];
		static const float EXTEND=-1, STOP=0, RETRACT=1;
		p.climber = Panel::Climber::EXTEND;
		AXIS_RANGE(climb, EXTEND, STOP, RETRACT, p.climber, Panel::Climber::STOP)
		else AXIS_RANGE(climb, STOP, RETRACT, 1.5, p.climber, Panel::Climber::RETRACT)
	}
	{
		float front = d.axis[7];
		static const float OUT=-1, OFF=.48, IN=1;
		p.front = Panel::Collector::OUT;
		AXIS_RANGE(front, OUT, OFF, IN, p.front, Panel::Collector::OFF)
		else AXIS_RANGE(front, OFF, IN, 1.5, p.front, Panel::Collector::IN)
	}
	{
		float sides = d.axis[5];
                static const float OUT=-1, OFF=0, IN=1;
                p.sides = Panel::Collector::OUT;
                AXIS_RANGE(sides, OUT, OFF, IN, p.sides, Panel::Collector::OFF)
                else AXIS_RANGE(sides, OFF, IN, 1.5, p.sides, Panel::Collector::IN)
	}
	{
		float tilt = d.axis[4];
		static const float UP=-1, STOP=0, DOWN=1;
		p.tilt = Panel::Tilt::UP;
		AXIS_RANGE(tilt, UP, STOP, DOWN, p.tilt, Panel::Tilt::STOP)
		else AXIS_RANGE(tilt, STOP, DOWN, 1.5, p.tilt, Panel::Tilt::DOWN);
	}
        {
                float winch = d.axis[3];//TODO: get actual number & switch +/-?
                static const float UP=-1, STOP=0, DOWN=1;
                p.winch = Panel::Winch::UP;
                AXIS_RANGE(winch, UP, STOP, DOWN, p.winch, Panel::Winch::STOP)
                else AXIS_RANGE(winch, STOP, DOWN, 1.5, p.winch, Panel::Winch::DOWN);
        }

	p.angle = d.axis[6];
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
