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
	
	private:
	unsigned int port;
	set<Volt> targets;
	
	public:
	unsigned int get()const;
	void interpret(const Volt);
	void interpret(const Joystick_data);
	friend ostream& operator<<(ostream&,Multistate_control);
	Multistate_control();
	explicit Multistate_control(Input_type,unsigned int,unsigned int);//pass in the number of positions and have it automatically create target values
	explicit Multistate_control(Input_type,unsigned int,set<Volt> set_targets);//pass in a set of target values
};

class Combo_multistate_control: public Multistate_control{//used for a three position switch connected to two digital inputs
	//TODO: maybe replace with handling inside of Multistate_control by checking to see if the input_type is BUTTON but it also expects more than 2 states
	public:
	struct Ports{
		unsigned int up;
		unsigned int down;
		Ports();
		Ports(unsigned int,unsigned int);
	};
	private:
	Ports ports;
	
	public:
	void interpret(const bool,const bool);
	void interpret(const Joystick_data);
	friend ostream& operator<<(ostream&,Combo_multistate_control);
	Combo_multistate_control();
	explicit Combo_multistate_control(Ports);
};

class Dial{
	float value;
	unsigned int port;
	
	public:
	friend ostream& operator<<(ostream&,Dial);
	float get()const;
	void interpret(const float);
	void interpret(const Joystick_data);
	float to_percent()const;
	Dial();
	explicit Dial(unsigned int);
};

struct Panel{
	enum Two_position_switches{LOCK_CLIMBER,TILT_AUTO,SIDES_AUTO,FRONT_AUTO,TWO_POSITION_SWITCH_NUMBER};
	enum Three_position_switches{WINCH,FRONT,COLLECTOR_POS,SIDES,THREE_POSITION_SWITCH_NUMBER};
	bool in_use;
	Multistate_control buttons;//Buttons
	array<Multistate_control,Two_position_switches::TWO_POSITION_SWITCH_NUMBER> two_position_switches;//2 position swicthes
	array<Multistate_control,Three_position_switches::THREE_POSITION_SWITCH_NUMBER> three_position_switches;//3 position switches
	Multistate_control auto_switch;//10 position switch
	Combo_multistate_control shooter_mode;//A three position switch connected to two digital inputs
	enum class Auto_mode{NOTHING,REACH,STATICF,STATICS,PORTCULLIS,CHEVAL,LBLS,LBWLS,LBWHS,S};//TODO: remove this and put it in main
	Auto_mode auto_mode;
	Dial speed_dial;//Dial
	Panel();
};

ostream& operator<<(ostream&,const Multistate_control::Input_type);
ostream& operator<<(ostream&,const Multistate_control);
ostream& operator<<(ostream&,Panel::Auto_mode);
ostream& operator<<(ostream&,const Panel);

Panel interpret(Joystick_data);

#endif
