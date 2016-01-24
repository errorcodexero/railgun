#ifndef PANEL2016_H
#define PANEL2016_H 

#include "../util/maybe.h"
#include "../util/interface.h"

struct Panel{
	bool in_use;
   	//Buttons:
	bool stop_climb;
	bool activate_tilt;
	//2 position switches: 
	enum class Climber{UP,DOWN};
	Climber climber;
	//4 position switches: 
	enum class Collector{IN,OUT,OFF,AUTO};
	Collector front, sides;
	enum class Tilt{UP,DOWN,STOP,AUTO};
	Tilt tilt;
	//10 position switches:
	enum class Auto_mode{NOTHING,MOVE,SHOOT};
	Auto_mode auto_mode;
	//Dial
	double angle;
	Panel();
};

std::ostream& operator<<(std::ostream&,Panel::Climber);
std::ostream& operator<<(std::ostream&,Panel::Collector);
std::ostream& operator<<(std::ostream&,Panel::Tilt);
std::ostream& operator<<(std::ostream&,Panel::Auto_mode);
std::ostream& operator<<(std::ostream&,Panel);

Panel interpret(Joystick_data);

#endif
