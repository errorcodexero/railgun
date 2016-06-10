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
	bool lock_climber;
	bool tilt_auto;
	bool front_auto;
	bool sides_auto;	
	//3 position switches: 
	enum class Collector_pos{STOW,DEFAULT,LOW};
	Collector_pos collector_pos;
	enum class Collector{IN,OFF,OUT};
	Collector front, sides;
	enum class Winch{UP,STOP,DOWN};
	Winch winch;
	enum class Shooter_mode{OPEN,CLOSED_MANUAL,CLOSED_AUTO};
	Shooter_mode shooter_mode;
	//10 position switches:

	//todo: remove this & put in main
	enum class Auto_mode{NOTHING,REACH,STATICF,STATICS,PORTCULLIS,CHEVAL,LBLS,LBWLS,LBWHS,S};
	Auto_mode auto_mode;
	Ten_position_pot auto_switch;//0-9

	//Dials:
	float speed_dial;
	Panel();
};

std::ostream& operator<<(std::ostream&,Ten_position_pot);
std::ostream& operator<<(std::ostream&,Panel::Collector_pos);
std::ostream& operator<<(std::ostream&,Panel::Collector);
std::ostream& operator<<(std::ostream&,Panel::Winch);
std::ostream& operator<<(std::ostream&,Panel::Shooter_mode);
std::ostream& operator<<(std::ostream&,Panel::Auto_mode);
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Joystick_data);

#endif
