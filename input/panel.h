#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"
#include <set>

class Multistate_input{//interprets from axes and sets value based on which segment of the range the voltage lies in
	protected:
	unsigned int value;
	unsigned int port;	
	std::set<Volt> targets;
	
	public:
	friend std::ostream& operator<<(std::ostream&,Multistate_input);
	unsigned int get()const;
	void interpret(const Volt);
	void interpret(const Joystick_data);
	//TODO: add invert function because the switches are mounted in different directions
	Multistate_input();
	explicit Multistate_input(unsigned int,unsigned int);//pass in the number of positions and have it automatically create target values
	explicit Multistate_input(unsigned int,std::set<Volt> set_targets);//pass in a set of target values
};

class Two_state_input{//Interprets from buttons not axes and therefore inherintly only has two states
	bool value;
	unsigned int port;
	
	public:
	friend std::ostream& operator<<(std::ostream&,Two_state_input);
	bool get()const;
	void interpret(const bool);
	void interpret(const Joystick_data);
	Two_state_input();
	explicit Two_state_input(unsigned int);
};

class Ten_position_switch: public Multistate_input{//Special case because the ten position pot on the oi is weird
	public:
	friend std::ostream& operator<<(std::ostream&,Ten_position_switch);
	void interpret(const Volt);
	void interpret(const Joystick_data);
	Ten_position_switch();
	explicit Ten_position_switch(unsigned int);
};

class Combo_switch: public Multistate_input{//used for a three position switch connected to two digital inputs
	//assume that the inherited "port" is the down port
	unsigned int up_port;
	
	public:
	friend std::ostream& operator<<(std::ostream&,Combo_switch);
	void interpret(const bool,const bool);
	void interpret(const Joystick_data);
	Combo_switch();
	explicit Combo_switch(unsigned int,unsigned int);
};

class Dial{
	float value;
	unsigned int port;
	
	public:
	friend std::ostream& operator<<(std::ostream&,Dial);
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
	enum Two_position_switches{LOCK_CLIMBER,TILT_AUTO,SIDES_AUTO,FRONT_AUTO,TWO_POSITION_SWITCH_NUMBER};
	enum Three_position_switches{WINCH,FRONT,COLLECTOR_POS,SIDES,THREE_POSITION_SWITCH_NUMBER};
	enum Three_position_switch{DOWN,MIDDLE,UP};
	Multistate_input buttons;//Buttons
	std::array<Two_state_input,Two_position_switches::TWO_POSITION_SWITCH_NUMBER> two_position_switches;//2 position swicthes
	std::array<Multistate_input,Three_position_switches::THREE_POSITION_SWITCH_NUMBER> three_position_switches;//3 position switches
	Ten_position_switch auto_switch;//10 position switch
	Combo_switch shooter_mode;//A three position switch connected to two digital inputs
	Dial speed_dial;//Dial
	Panel();
};

std::ostream& operator<<(std::ostream&,const Multistate_input);
std::ostream& operator<<(std::ostream&,const Panel);

bool operator!=(Panel const&,Panel const&);
/*std::ostream& operator<<(std::ostream&,Panel::Collector_pos);
std::ostream& operator<<(std::ostream&,Panel::Collector);
std::ostream& operator<<(std::ostream&,Panel::Winch);
std::ostream& operator<<(std::ostream&,Panel::Shooter_mode);
std::ostream& operator<<(std::ostream&,Panel::Auto_mode);*/
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Joystick_data);
Joystick_data driver_station_input_rand();
Panel rand(Panel*);

#endif
