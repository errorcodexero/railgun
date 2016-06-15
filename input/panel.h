#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"
#include <set>

using namespace std;

const set<Volt> TEN_POS_POT_TARGETS={-1.00,-0.75,-0.50,-0.25,0.00,0.20,0.40,0.60,0.80,1.00};

class Multistate_control{
	unsigned int value;
	set<Volt> targets;
	public:
	unsigned int get();
	void interpret(const Volt);
	friend std::ostream& operator<<(std::ostream&,Multistate_control);
	Multistate_control();
	explicit Multistate_control(unsigned int);//pass in the number of positions and have it automatically create target values
	explicit Multistate_control(set<Volt> set_targets);//pass in a set of target values
};

typedef bool Button;

struct Panel{
	bool in_use;
	//Buttons:
	Button learn;
	Button cheval;
	Button drawbridge;
	Button shoot_prep;
	Button shoot_low;
	Button collect;
	Button shoot_high;
	Button collector_up;
	Button collector_down;
	//2 position swicthes:
	Multistate_control lock_climber;
	Multistate_control tilt_auto;
	Multistate_control front_auto;
	Multistate_control sides_auto;
	//3 position switches: 
	Multistate_control collector_pos;
	Multistate_control front;
	Multistate_control sides;
	Multistate_control winch;
	Multistate_control shooter_mode;
	//10 position switches:
	Multistate_control auto_switch;
	//TODO: remove this and put it in main:
	enum class Auto_mode{NOTHING,REACH,STATICF,STATICS,PORTCULLIS,CHEVAL,LBLS,LBWLS,LBWHS,S};
	Auto_mode auto_mode;
	//Dials:
	float speed_dial;
	Panel();
};

std::ostream& operator<<(std::ostream&,Multistate_control);
std::ostream& operator<<(std::ostream&,Panel::Auto_mode);
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Joystick_data);

#endif
