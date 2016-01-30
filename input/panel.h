#ifndef PANEL_H
#define PANEL_H 

#include "../util/maybe.h"
#include "../util/interface.h"

struct Panel{
	bool in_use;
   	//Buttons:
	bool control_angle;
	bool eject;
	bool collect;
	bool reflect;
	bool stow;
	bool open_portcullis;
	bool lower_cheval;
	//2 position swicthes:
	bool collector_auto;
	//3 position switches: 
	enum class Climber{EXTEND,STOP,RETRACT};
	Climber climber; 
	enum class Collector{IN,OFF,OUT};
	Collector front, sides;
	enum class Tilt{UP,STOP,DOWN};
	Tilt tilt;
	//10 position switches:
	enum class Auto_mode{NOTHING,MOVE,SHOOT};
	Auto_mode auto_mode;
	//Dials
	float angle;
	Panel();
};

std::ostream& operator<<(std::ostream&,Panel::Climber);
std::ostream& operator<<(std::ostream&,Panel::Collector);
std::ostream& operator<<(std::ostream&,Panel::Tilt);
std::ostream& operator<<(std::ostream&,Panel::Auto_mode);
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Joystick_data);

#endif
