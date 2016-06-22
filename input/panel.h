#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"
#include <set>

using namespace std;

class Multistate_input{//interprets from axes and sets value based on which segment of the range the voltage lies in
	protected:
	unsigned int value;
	unsigned int port;	
	set<Volt> targets;
	
	public:
	friend ostream& operator<<(ostream&,Multistate_input);
	unsigned int get()const;
	void interpret(const Volt);
	void interpret(const Joystick_data);
	//TODO: add invert function because the switches are mounted in different directions
	Multistate_input();
	explicit Multistate_input(unsigned int,unsigned int);//pass in the number of positions and have it automatically create target values
	explicit Multistate_input(unsigned int,set<Volt> set_targets);//pass in a set of target values
};

class Two_state_input{//Interprets from buttons not axes and therefore inherintly only has two states
	bool value;
	unsigned int port;
	
	public:
	friend ostream& operator<<(ostream&,Two_state_input);
	bool get()const;
	void interpret(const bool);
	void interpret(const Joystick_data);
	Two_state_input();
	explicit Two_state_input(unsigned int);
};

class Ten_position_switch: public Multistate_input{//Special case because the ten position pot on the oi is weird
	public:
	friend ostream& operator<<(ostream&,Ten_position_switch);
	using Multistate_input::interpret;
	void interpret(const Volt);
	Ten_position_switch();
	explicit Ten_position_switch(unsigned int);
};

class Combo_switch: public Multistate_input{//used for a three position switch connected to two digital inputs
	//assume that the inherited "port" is the down port
	unsigned int up_port;
	
	public:
	friend ostream& operator<<(ostream&,Combo_switch);
	void interpret(const bool,const bool);
	void interpret(const Joystick_data);
	Combo_switch();
	explicit Combo_switch(unsigned int,unsigned int);
};

class Dial{
	float value;
	unsigned int port;
	
	public:
	friend ostream& operator<<(ostream&,Dial);
	float get()const;
	void interpret(const Volt);
	void interpret(const Joystick_data);
	float to_percent()const;
	Dial();
	explicit Dial(unsigned int);
};

struct Panel{
	bool in_use;
	enum Buttons{DEFAULT,COLLECTOR_UP,COLLECTOR_DOWN,SHOOT_HIGH,COLLECT,SHOOT_LOW,SHOOT_PREP,DRAWBRIDGE,CHEVAL,LEARN};
	enum Two_state_inputes{LOCK_CLIMBER,TILT_AUTO,SIDES_AUTO,FRONT_AUTO,TWO_POSITION_SWITCH_NUMBER};
	enum Three_position_switches{WINCH,FRONT,COLLECTOR_POS,SIDES,THREE_POSITION_SWITCH_NUMBER};
	enum Three_position_switch{DOWN,MIDDLE,UP};
	Multistate_input buttons;//Buttons
	array<Two_state_input,Two_state_inputes::TWO_POSITION_SWITCH_NUMBER> two_position_switches;//2 position swicthes
	array<Multistate_input,Three_position_switches::THREE_POSITION_SWITCH_NUMBER> three_position_switches;//3 position switches
	Ten_position_switch auto_switch;//10 position switch
	Combo_switch shooter_mode;//A three position switch connected to two digital inputs
	Dial speed_dial;//Dial
	Panel();
};

ostream& operator<<(ostream&,const Multistate_input);
ostream& operator<<(ostream&,const Panel);

Panel interpret(Joystick_data);

#endif
