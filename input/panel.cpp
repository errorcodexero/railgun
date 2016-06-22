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
const unsigned int SHOOTER_MODE_PORT_DOWN=5;
const unsigned int SHOOTER_MODE_PORT_UP=6;

Panel::Panel():
	in_use(false),
	buttons(Multistate_control::Input_type::AXIS,BUTTON_PORT,BUTTON_TARGETS),
	two_position_switches(),
	three_position_switches(),
	auto_switch(Multistate_control::Input_type::AXIS,AUTO_SWITCH_PORT,TEN_POS_POT_TARGETS),
	shooter_mode(SHOOTER_MODE_PORT_DOWN,SHOOTER_MODE_PORT_UP),
	speed_dial(DIAL_PORT)
{
	#define X(POSITION) two_position_switches[Panel::Two_position_switches::POSITION]=Multistate_control(Multistate_control::Input_type::BUTTON,2,TWO_POSITION_SWITCH_PORTS[Panel::Two_position_switches::POSITION]);
	X(LOCK_CLIMBER) X(TILT_AUTO) X(SIDES_AUTO) X(FRONT_AUTO)
	#undef X
	
	#define X(POSITION) three_position_switches[Panel::Three_position_switches::POSITION]=Multistate_control(Multistate_control::Input_type::AXIS,3,THREE_POSITION_SWITCH_PORTS[Panel::Three_position_switches::POSITION]);
	X(WINCH) X(FRONT) X(COLLECTOR_POS) X(SIDES)
	#undef X
}

Combo_switch::Combo_switch():Multistate_control(Input_type::COMBO,0,0),up_port(1){}
Combo_switch::Combo_switch(unsigned int p_down,unsigned int p_up):Multistate_control(Input_type::COMBO,p_down,0),up_port(p_up){}

void Combo_switch::interpret(const bool DOWN,const bool UP){
	if(DOWN)value=0;
	else if(UP) value=2;
	else value=1;
}

void Combo_switch::interpret(const Joystick_data d){
	interpret(d.button[port],d.button[up_port]);
}

ostream& operator<<(ostream& o,const Combo_switch a){
	//o<<"Combo_switch(";
	o<<"(";
	o<<"value:"<<a.value;
	o<<" targets:N/A";
	o<<" ports:(down:"<<a.port<<" up:"<<a.up_port<<")";
	o<<" input_type:"<<a.input_type;
	return o<<")";
}

Dial::Dial():value(0.0),port(0){}
Dial::Dial(unsigned int p):value(0.0),port(p){}

float Dial::get()const{
	return value;
}

float Dial::to_percent()const{
	return .5-(value/2);
}

void Dial::interpret(const float a){
	value=a;
}
void Dial::interpret(const Joystick_data d){
	value=d.axis[port];
}

ostream& operator<<(ostream& o,const Dial a){
	//o<<"Dial(";
	o<<"(";
	o<<"value:"<<a.value;
	o<<" port:"<<a.port;
	return o<<")";
}

float mid(const float a,const float b){
	return a+(b-a)/2;
}

Multistate_control::Multistate_control():value(0),input_type(Input_type::BUTTON),port(0),targets({}){}
Multistate_control::Multistate_control(Input_type in,unsigned int p,unsigned int size):value(0),input_type(in),port(p),targets({}){
	switch(input_type){
		case Multistate_control::Input_type::BUTTON:
			targets={0,1};
			break;
		case Multistate_control::Input_type::AXIS:
			{
				assert(size>1);
				float target=-1;//set initial target to minimum possible value
				target+=1/(float)size;//set target to 1/size (half of the size of each range of targets)
				for(unsigned int i=0; i<size; i++){
					targets.insert(target);
					target+=2/(float)size;//add 2/size (the size of each range) each time until each target has been assigned
				}
				break;
			}
		case Multistate_control::Input_type::COMBO:
			//N/A?
			break;
		default:
			assert(0);
	}
}
Multistate_control::Multistate_control(Input_type in,unsigned int p,set<Volt> set_targets):value(0),input_type(in),port(p),targets({}){	
	switch(input_type){
		case Multistate_control::Input_type::BUTTON:
			targets={0,1};
			break;
		case Multistate_control::Input_type::AXIS:
			assert(set_targets.size()>1);
			targets=set_targets;
			break;
		case Multistate_control::Input_type::COMBO:
			//N/A?
			break;
		default:
			assert(0);
	}
}

void Multistate_control::interpret(const Joystick_data d){
	switch(input_type){
		case Multistate_control::Input_type::BUTTON:
			interpret(d.button[port]);
			break;
		case Multistate_control::Input_type::AXIS:
			interpret(d.axis[port]);
			break;
		case Multistate_control::Input_type::COMBO:
			//handled elsewhere for now
			break;
		default: assert(0);
	}
}

void Multistate_control::interpret(const Volt volt){
	vector<Volt> temp;
	for(auto i=targets.begin(); i!=targets.end(); i++) temp.push_back(*i);
	temp.insert(temp.begin(),-2.0);//add a minimum target (won't ever be set to this) so that there's a lower midpoint to test for
	temp.insert(temp.end(),2.0);//add a maximum target (won't ever be set to this) so that there's an upper midpoint to test for
	for(unsigned int i=1; i<temp.size()-1; i++){
		if(volt >= mid(temp[i-1],temp[i]) && volt < mid(temp[i],temp[i+1])){//if the axis value is more than its midpoint with the previous one and less than than its midpoint between the next one, then return that value
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
	X(AXIS) X(BUTTON) X(COMBO)
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
	
	p.speed_dial.interpret(-d.axis[DIAL_PORT]);
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
		cout<<"Test value   multistate_control(3)\n";
		for(float i=-1; i<=1; i+=.01){
			if(i>=0)cout<<" ";
			cout<<std::fixed<<std::setprecision(2)<<i<<"         ";
			Multistate_control a(Multistate_control::Input_type::AXIS,0,3);
			a.interpret(i);
			cout<<a.get()<<"\n";
		}
		cout<<"\n\n";
	}
	
	Panel p;
	srand(time(NULL));
	for(unsigned i=0;i<50;i++){
		p=interpret(driver_station_input_rand());
	}
	cout<<p<<"\n";
	return 0;
}
#endif
