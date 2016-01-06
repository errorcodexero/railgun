#ifndef MAIN_H
#define MAIN_H

#include "force_interface.h"
#include "../util/posedge_toggle.h"
#include "../util/perf_tracker.h"
#include "../util/countdown_timer.h"
#include "../util/countup_timer.h"
#include "toplevel.h"
#include "../input/panel2015.h"

struct Main{
	#define MODES X(TELEOP) X(AUTO_MOVE) 
	enum class Mode{
		#define X(NAME) NAME,
		MODES
		#undef X
	};
	Mode mode;
	
	Force_interface force;
	Perf_tracker perf;
	Toplevel toplevel;

	Countup_timer since_switch,since_auto_start;

	Posedge_trigger autonomous_start;

	Posedge_toggle piston;
	
	struct Nudge{
		Posedge_trigger trigger;
		Countdown_timer timer;
	};
	Nudge nudges[4];//Forward, Backward, Clockwise, Counter-clockwise
		
	enum class Button_mode{MANUAL,AUTO_UP,AUTO_DOWN};
	Button_mode button_mode;

	Toplevel::Goal teleop(Robot_inputs const&,Joystick_data const&,Joystick_data const&,Panel const&,Toplevel::Status_detail&);
	Main();
	Robot_outputs operator()(Robot_inputs,std::ostream& = std::cerr);
};

std::ostream& operator<<(std::ostream&,Main::Mode);

bool operator==(Main,Main);
bool operator!=(Main,Main);
std::ostream& operator<<(std::ostream&,Main);

#endif
