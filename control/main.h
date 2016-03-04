#ifndef MAIN_H
#define MAIN_H

#include "force_interface.h"
#include "../util/posedge_toggle.h"
#include "../util/perf_tracker.h"
#include "../util/countdown_timer.h"
#include "../util/countup_timer.h"
#include "toplevel.h"
#include "../input/panel.h"
#include "../util/nav.h"

struct Main{
	#define MODES X(TELEOP) X(AUTO_MOVE) X(AUTO_NAV) X(AUTO_NAV_RUN) X(AUTO_NULL) X(AUTO_SCORE) X(AUTO_REACH) X(AUTO_STOP)
	enum class Mode{
		#define X(NAME) NAME,
		MODES
		#undef X
	};
	Mode mode;

	struct NavS{
		float left; 
		float right;
		float amount;
	};
	struct navloadinput{
		point ptone;
		point pttwo;
		direction dirone;
		direction dirtwo;
	};
	
	struct navinput{
		point navpt;
		direction navdir;
	};
	struct aturn{
		float l;
		float r;
		float dur;
	};

	int stepcounter;
	navloadinput s1;
	navloadinput s2;
	navloadinput s3;
	navloadinput s4;
	navloadinput s5;
	
	aturn Aturn;

	
	unsigned int navindex;
	std::vector<NavS> NavV;
		
	vector<NavS> loadnav(navloadinput navin);	

	Force_interface force;
	Perf_tracker perf;
	Toplevel toplevel;
	
	Countup_timer since_switch,since_auto_start;

	Posedge_trigger autonomous_start;

	enum Nudges{FORWARD,BACKWARD,CLOCKWISE,COUNTERCLOCKWISE,NUDGES};	
	struct Nudge{
		Posedge_trigger trigger;
		Countdown_timer timer;
	};
	Nudge nudges[NUDGES];
	
	#define JOY_COLLECTOR_POS X(STOP) X(LOW) X(LEVEL)
	enum class Joy_collector_pos{
		#define X(name) name,
		JOY_COLLECTOR_POS
		#undef X
	};
	Joy_collector_pos joy_collector_pos;
		
	Posedge_toggle controller_auto;
	#define COLLECTOR_MODES X(NOTHING) X(COLLECT) X(STOW) X(EJECT) X(REFLECT) X(SHOOT) X(LOW)
	enum class Collector_mode{
		#define X(name) name,
		COLLECTOR_MODES
		#undef X
	};
	Collector_mode collector_mode;
	
	Countdown_timer shoot_timer;
	
	enum Panel_outputs{SHOOTER_PREPPED, BOULDER, PANEL_OUTPUTS};
	enum class Panel_output_ports{SHOOTER_PREPPED=6, BOULDER=10};

	Checked_array<Panel_output,Panel_outputs::PANEL_OUTPUTS> main_panel_output;
		
	Toplevel::Goal teleop(Robot_inputs const&,Joystick_data const&,Joystick_data const&,Panel const&,Toplevel::Status_detail&);
	Main();
	Robot_outputs operator()(Robot_inputs,std::ostream& = std::cerr);
};

std::ostream& operator<<(std::ostream&,Main::Mode);

bool operator==(Main,Main);
bool operator!=(Main,Main);
std::ostream& operator<<(std::ostream&,Main);

#endif
