#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"
#include <set>

using namespace std;

class Multistate_control{
	public:
	enum class Input_type{BUTTON,AXIS};
	
	private:
	unsigned int value;
	set<Volt> targets;
	unsigned int port;
	Input_type input_type;
	
	public:
	unsigned int get()const;
	void interpret(const Volt);
	void interpret(const Joystick_data);
	friend ostream& operator<<(ostream&,Multistate_control);
	Multistate_control();
	explicit Multistate_control(unsigned int,unsigned int,Input_type);//pass in the number of positions and have it automatically create target values
	explicit Multistate_control(set<Volt> set_targets,unsigned int,Input_type);//pass in a set of target values
};

class Two_digital_switch{//used for a three position switch connected to two digital inputs
	public:
	struct Ports{
		unsigned int up;
		unsigned int down;
		Ports();
		Ports(unsigned int,unsigned int);
	};
	private:	
	unsigned int value;
	Ports ports;

	public:
	unsigned int get()const;
	void interpret(const bool,const bool);
	void interpret(const Joystick_data);
	friend ostream& operator<<(ostream&,Two_digital_switch);
	Two_digital_switch();
	explicit Two_digital_switch(Ports);	
};

struct Panel{
	enum Two_position_switches{LOCK_CLIMBER,TILT_AUTO,SIDES_AUTO,FRONT_AUTO,TWO_POSITION_SWITCH_NUMBER};
	enum Three_position_switches{WINCH,FRONT,COLLECTOR_POS,SIDES,THREE_POSITION_SWITCH_NUMBER};
	bool in_use;
	Multistate_control buttons;//Buttons
	array<Multistate_control,Two_position_switches::TWO_POSITION_SWITCH_NUMBER> two_position_switches;//2 position swicthes
	array<Multistate_control,Three_position_switches::THREE_POSITION_SWITCH_NUMBER> three_position_switches;//3 position switches
	Multistate_control auto_switch;//10 position switch
	Two_digital_switch shooter_mode;//A three position switch connected to two digital inputs
	enum class Auto_mode{NOTHING,REACH,STATICF,STATICS,PORTCULLIS,CHEVAL,LBLS,LBWLS,LBWHS,S};//TODO: remove this and put it in main
	Auto_mode auto_mode;
	float speed_dial;//Dial
	Panel();
};

ostream& operator<<(ostream&,const Multistate_control::Input_type);
ostream& operator<<(ostream&,const Multistate_control);
ostream& operator<<(ostream&,Panel::Auto_mode);
ostream& operator<<(ostream&,const Panel);

Panel interpret(Joystick_data);

#endif
