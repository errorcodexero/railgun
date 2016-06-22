#include "panel.h"
#include <iostream>
#include <stdlib.h> 
#include "util.h"
#include "../util/util.h"
#include <cmath>
#include <iomanip> 

using namespace std;

const set<Volt> TEN_POS_POT_TARGETS={-1.00,-0.75,-0.50,-0.25,0.00,0.20,0.40,0.60,0.80,1.00};
const set<Volt> BUTTON_TARGETS={-1,-.8,-.62,-.45,-.29,-.11,.09,.33,.62,1};

const unsigned int DIAL_PORT=1;
const unsigned int BUTTON_PORT=2;
const unsigned int AUTO_SWITCH_PORT=0;
const array<unsigned int,Panel::Two_position_switches::TWO_POSITION_SWITCH_NUMBER> TWO_POSITION_SWITCH_PORTS={0,1,2,3};
const array<unsigned int,Panel::Three_position_switches::THREE_POSITION_SWITCH_NUMBER> THREE_POSITION_SWITCH_PORTS={3,4,5,6};
const Two_digital_switch::Ports SHOOTER_MODE_PORTS={5,6};

Panel::Panel():
	in_use(false),
	buttons(BUTTON_TARGETS,BUTTON_PORT,Multistate_control::Input_type::AXIS),
	two_position_switches(),
	three_position_switches(),
	auto_switch(TEN_POS_POT_TARGETS,AUTO_SWITCH_PORT,Multistate_control::Input_type::AXIS),
	shooter_mode(SHOOTER_MODE_PORTS),
	speed_dial(0.0)
{
	#define X(POSITION) two_position_switches[Panel::Two_position_switches::POSITION]=Multistate_control(2,TWO_POSITION_SWITCH_PORTS[Panel::Two_position_switches::POSITION],Multistate_control::Input_type::BUTTON);
	X(LOCK_CLIMBER) X(TILT_AUTO) X(SIDES_AUTO) X(FRONT_AUTO)
	#undef X
	
	#define X(POSITION) three_position_switches[Panel::Three_position_switches::POSITION]=Multistate_control(3,THREE_POSITION_SWITCH_PORTS[Panel::Three_position_switches::POSITION],Multistate_control::Input_type::AXIS);
	X(WINCH) X(FRONT) X(COLLECTOR_POS) X(SIDES)
	#undef X
}

Two_digital_switch::Ports::Ports():up(0),down(0){}
Two_digital_switch::Ports::Ports(unsigned int a,unsigned int b):up(a),down(b){}

Two_digital_switch::Two_digital_switch():value(0),ports({0,1}){}
Two_digital_switch::Two_digital_switch(Ports p):value(0),ports(p){
	assert(p.up!=p.down);
}

unsigned int Two_digital_switch::get()const{
	return value;
}

void Two_digital_switch::interpret(const bool down,const bool up){
	if(down) value=0;
	else if(up) value=2;
	else value=1;
}

void Two_digital_switch::interpret(const Joystick_data d){
	interpret(d.button[ports.down],d.button[ports.up]);
}

ostream& operator<<(ostream& o,const Two_digital_switch::Ports a){
	//o<<"Ports(";
	o<<"(";
	o<<"up:"<<a.up;
	o<<" down:"<<a.down;
	return o<<")";
}

ostream& operator<<(ostream& o,const Two_digital_switch a){
	//o<<"Two_digital_switch(";
	o<<"(";
	o<<"value:"<<a.value;
	o<<" ports:"<<a.ports;
	return o<<")";
}

float mid(const float a,const float b){
	return a+(b-a)/2;
}

Multistate_control::Multistate_control():value(0),targets({}),port(0),input_type(Input_type::BUTTON){}
Multistate_control::Multistate_control(unsigned int size,unsigned int p,Input_type in):value(0),targets({}),port(p),input_type(in){
	assert(size>1);
	for(unsigned int i=0; i<size; i++){
		targets.insert(-1+((1+2*i)/(float)size));
	}
}
Multistate_control::Multistate_control(set<Volt> set_targets,unsigned int p,Input_type in):value(0),targets({}),port(p),input_type(in){
	assert(set_targets.size()>1);
	targets=set_targets;
}

void Multistate_control::interpret(const Joystick_data d){
	switch(input_type){
		case Multistate_control::Input_type::BUTTON:
			interpret(d.button[port]);
			break;
		case Multistate_control::Input_type::AXIS:
			interpret(d.axis[port]);
			break;
		default: assert(0);
	}
}

void Multistate_control::interpret(const Volt volt){
	vector<Volt> temp;
	for(auto i=targets.begin(); i!=targets.end(); i++) temp.push_back(*i);
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

unsigned int Multistate_control::get()const{
	return value;
}

ostream& operator<<(ostream& o,const Multistate_control::Input_type a){
	#define X(NAME) if(a==Multistate_control::Input_type::NAME) return o<<""#NAME;
	X(AXIS) X(BUTTON)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,const Multistate_control a){
	//o<<"Multistate_control";
	o<<"(";
	o<<"value:"<<a.value;
	o<<" targets:"<<a.targets;
	o<<" port:"<<a.port;
	o<<" input_type:"<<a.input_type;
	return o<<")";
}

ostream& operator<<(ostream& o,const Panel p){
	o<<"Panel(";
	o<<"in_use:"<<p.in_use;
	o<<", buttons:"<<p.buttons.get();//buttons
	for(unsigned int i=0; i<Panel::Two_position_switches::TWO_POSITION_SWITCH_NUMBER; i++){//2-pos switches
		o<<", two_position_switches["<<i<<"]:"<<p.two_position_switches[i].get();
	}
	for(unsigned int i=0; i<Panel::Three_position_switches::THREE_POSITION_SWITCH_NUMBER; i++){//3-pos switches
		o<<", three_position_switches["<<i<<"]:"<<p.three_position_switches[i].get();
	}
	o<<", auto_switch:"<<p.auto_switch.get();//10-pos switches
	#define X(NAME) o<<", "#NAME":"<<p.NAME;
	X(shooter_mode)
	X(speed_dial) //Dials
	#undef X
	return o<<")";
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
	p.auto_switch.interpret(d);
	
	p.buttons.interpret(d);
	
	for(unsigned int i=0; i<Panel::Two_position_switches::TWO_POSITION_SWITCH_NUMBER; i++){
		p.two_position_switches[i].interpret(d);
	}
	for(unsigned int i=0; i<Panel::Three_position_switches::THREE_POSITION_SWITCH_NUMBER; i++){
		p.three_position_switches[i].interpret(d);
	}
	
	p.shooter_mode.interpret(d);
	
	p.speed_dial = -d.axis[DIAL_PORT];//axis_to_percent(-d.axis[1]);
	return p;
}

#ifdef PANEL_TEST
Joystick_data driver_station_input_rand(){
	Joystick_data r;
	for(unsigned i=0;i<JOY_AXES;i++){
		srand(time(NULL));
		r.axis[i]=(0.0+rand()%101)/100;
	}
	for(unsigned i=0;i<JOY_BUTTONS;i++){
		srand(time(NULL));
		r.button[i]=rand()%2;
	}
	return r;
}

int main(){
	{
		cout<<"Test value   multistate_control(3)\n";
		for(float i=-1; i<=1; i+=.01){
			if(i>=0)cout<<" ";
			cout<<std::fixed<<std::setprecision(2)<<i<<"         ";
			Multistate_control a(3,0,Multistate_control::Input_type::AXIS);
			a.interpret(i);
			cout<<a.get()<<"\n";
		}
		cout<<"\n\n";
	}
	
	Panel p;
	for(unsigned i=0;i<50;i++){
		p=interpret(driver_station_input_rand());
	}
	cout<<p<<"\n";
	return 0;
}
#endif
