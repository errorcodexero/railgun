#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"

const std::array<Volt,10> TEN_POS_POT_LIMITS={-1.00,-0.75,-0.50,-0.25,0.00,0.20,0.40,0.60,0.80,1.00};

class Ten_position_pot{
	unsigned int value;
	std::array<Volt,10> limits;
	public:
	unsigned int get();
	void interpret(const Volt);
	std::array<Volt,10> get_limits();
	Ten_position_pot();
	Ten_position_pot(Volt);
	explicit Ten_position_pot(std::array<Volt,10> set_limits);
};

typedef bool Button;

enum class Three_position_switch{DOWN,MIDDLE,UP};
enum class Two_position_switch{DOWN,UP};

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
	Two_position_switch lock_climber;
	Two_position_switch tilt_auto;
	Two_position_switch front_auto;
	Two_position_switch sides_auto;
	//3 position switches: 
	Three_position_switch collector_pos;
	Three_position_switch front;
	Three_position_switch sides;
	Three_position_switch winch;
	Three_position_switch shooter_mode;
	//10 position switches:
	enum class Auto_mode{NOTHING,REACH,STATICF,STATICS,PORTCULLIS,CHEVAL,LBLS,LBWLS,LBWHS,S};//todo: remove this & put in main
	Auto_mode auto_mode;
	Ten_position_pot auto_switch;//0-9
	//Dials:
	float speed_dial;
	Panel();
};

std::ostream& operator<<(std::ostream&,Ten_position_pot);
std::ostream& operator<<(std::ostream&,Two_position_switch);
std::ostream& operator<<(std::ostream&,Three_position_switch);
std::ostream& operator<<(std::ostream&,Panel::Auto_mode);
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Joystick_data);

#endif
