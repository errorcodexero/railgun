#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"
#include <set>

using namespace std;

const set<Volt> TEN_POS_POT_TARGETS={-1.00,-0.75,-0.50,-0.25,0.00,0.20,0.40,0.60,0.80,1.00};
const set<Volt> BUTTON_TARGETS={-1,-.8,-.62,-.45,-.29,-.11,.09,.33,.62,1};
const unsigned int BUTTON_PORT=2;
const unsigned int AUTO_SWITCH_PORT=0;
enum Two_position_switches{LOCK_CLIMBER,TILT_AUTO,SIDES_AUTO,FRONT_AUTO,TWO_POSITION_SWITCH_NUMBER};
const array<unsigned int,Two_position_switches::TWO_POSITION_SWITCH_NUMBER> TWO_POSITION_SWITCH_PORTS={0,1,2,3};
enum  Three_position_switches{WINCH,FRONT,COLLECTOR_POS,SIDES,THREE_POSITION_SWITCH_NUMBER};
const array<unsigned int,Three_position_switches::THREE_POSITION_SWITCH_NUMBER> THREE_POSITION_SWITCH_PORTS={3,4,5,6};

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

struct Panel{
	bool in_use;
	Multistate_control buttons;//Buttons
	array<Multistate_control,Two_position_switches::TWO_POSITION_SWITCH_NUMBER> two_position_switches;//2 position swicthes
	array<Multistate_control,Three_position_switches::THREE_POSITION_SWITCH_NUMBER> three_position_switches;//3 position switches
	Multistate_control auto_switch;//10 position switches
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
