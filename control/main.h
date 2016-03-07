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
	vector<NavS> NavV;
		
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
	array<Nudge,NUDGES> nudges;
	
	#define JOY_COLLECTOR_POS X(STOP) X(LOW) X(LEVEL)
	enum class Joy_collector_pos{
		#define X(name) name,
		JOY_COLLECTOR_POS
		#undef X
	};
	Joy_collector_pos joy_collector_pos;
		
	Posedge_toggle controller_auto;
	#define COLLECTOR_MODES X(CHEVAL) X(PORTCULLIS) X(NOTHING) X(COLLECT) X(STOW) X(EJECT) X(REFLECT) X(SHOOT) X(LOW)
	enum class Collector_mode{
		#define X(name) name,
		COLLECTOR_MODES
		#undef X
	};
	Collector_mode collector_mode;
	
	Countdown_timer learn_delay;

	Countdown_timer shoot_timer, cheval_lift_timer, cheval_drive_timer, portcullis_timer;
	
	enum class Cheval_steps{GO_DOWN, DRIVE, DRIVE_AND_STOW};
	Cheval_steps cheval_step;
	
	Posedge_toggle learn;

	Checked_array<Panel_output,Panel_outputs::PANEL_OUTPUTS> main_panel_output;
		
	double top, level, low, cheval, portcullis;//angles (in degrees) that it will go to when set to the tilt goals

	Toplevel::Goal teleop(Robot_inputs const&,Joystick_data const&,Joystick_data const&,Panel const&,Toplevel::Status_detail&);
	Main();
	Robot_outputs operator()(Robot_inputs,std::ostream& = std::cerr);
};

std::ostream& operator<<(std::ostream&,Main::Mode);

bool operator==(Main,Main);
bool operator!=(Main,Main);
std::ostream& operator<<(std::ostream&,Main);

#endif
