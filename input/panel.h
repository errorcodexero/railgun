#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"

const std::vector<Volt> TEN_POS_POT_LIMITS={-1.00,-0.75,-0.50,-0.25,0.00,0.20,0.40,0.60,0.80,1.00};

class Switch{
	unsigned int value;
	std::vector<Volt> targets;
	public:
	unsigned int get();
	void interpret(const Volt);
	friend std::ostream& operator<<(std::ostream&,Switch);
	Switch();
	explicit Switch(unsigned int);
	explicit Switch(std::vector<Volt> set_targets);
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
	Switch lock_climber;
	Switch tilt_auto;
	Switch front_auto;
	Switch sides_auto;
	//3 position switches: 
	Switch collector_pos;
	Switch front;
	Switch sides;
	Switch winch;
	Switch shooter_mode;
	//10 position switches:
	enum class Auto_mode{NOTHING,REACH,STATICF,STATICS,PORTCULLIS,CHEVAL,LBLS,LBWLS,LBWHS,S};//todo: remove this & put in main
	Auto_mode auto_mode;
	Switch auto_switch;
	//Dials:
	float speed_dial;
	Panel();
};

std::ostream& operator<<(std::ostream&,Switch);
std::ostream& operator<<(std::ostream&,Panel::Auto_mode);
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Joystick_data);

#endif
