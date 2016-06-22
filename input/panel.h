#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"
#include <set>

using namespace std;

class Multistate_control{
	public:
	enum class Input_type{BUTTON,AXIS,COMBO};
	
	protected:
	unsigned int value;
	Input_type input_type;
	unsigned int port;
	
	private:
	set<Volt> targets;
	
	public:
	friend ostream& operator<<(ostream&,Multistate_control);
	unsigned int get()const;
	void interpret(const Volt);
	void interpret(const Joystick_data);
	Multistate_control();
	explicit Multistate_control(Input_type,unsigned int,unsigned int);//pass in the number of positions and have it automatically create target values
	explicit Multistate_control(Input_type,unsigned int,set<Volt> set_targets);//pass in a set of target values
};

class Button: public Multistate_control{
	public:
	friend ostream& operator<<(ostream&,Button);
	void interpret(const bool);
	void interpret(const Joystick_data);
	Button();
	explicit Button(unsigned int);
};


class Ten_position_switch: public Multistate_control{//Special case because the ten position pot on the oi is weird
	set<Volt> targets;
	public:
	friend ostream& operator<<(ostream&,Ten_position_switch);
	void interpret(const Volt);
	Ten_position_switch();
	explicit Ten_position_switch(unsigned int);
};

class Combo_switch: public Multistate_control{//used for a three position switch connected to two digital inputs
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
	enum Two_position_switches{LOCK_CLIMBER,TILT_AUTO,SIDES_AUTO,FRONT_AUTO,TWO_POSITION_SWITCH_NUMBER};
	enum Three_position_switches{WINCH,FRONT,COLLECTOR_POS,SIDES,THREE_POSITION_SWITCH_NUMBER};
	enum class Two_position_switch{DOWN,UP};
	enum class Three_position_switch{DOWN,MIDDLE,UP};
	Multistate_control buttons;//Buttons
	array<Multistate_control,Two_position_switches::TWO_POSITION_SWITCH_NUMBER> two_position_switches;//2 position swicthes
	array<Multistate_control,Three_position_switches::THREE_POSITION_SWITCH_NUMBER> three_position_switches;//3 position switches
	Multistate_control auto_switch;//10 position switch
	Combo_switch shooter_mode;//A three position switch connected to two digital inputs
	Dial speed_dial;//Dial
	Panel();
};

ostream& operator<<(ostream&,const Multistate_control::Input_type);
ostream& operator<<(ostream&,const Multistate_control);
ostream& operator<<(ostream&,const Panel);

Panel interpret(Joystick_data);

#endif
