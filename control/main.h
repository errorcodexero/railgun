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
#include "../util/nav2.h"
#include "log.h"

struct Tilt_presets{
	double top, level, low, cheval, portcullis;//angles (in degrees) that it will go to when set to the tilt goals
	Tilt_presets();
};

struct Main{
	#define MODES X(TELEOP) X(AUTO_MOVE) X(AUTO_NAV) X(AUTO_NAV_RUN) \
		X(AUTO_NULL) X(AUTO_SCORE) X(AUTO_REACH) X(AUTO_STATIC) \
		X(AUTO_STOP) X(AUTO_STATICTWO) X(AUTO_TEST) \
		X(AUTO_PORTCULLIS) X(AUTO_CHEVAL)

	enum class Mode{
		#define X(NAME) NAME,
		MODES
		#undef X
	};
	Mode mode;

	Nav2 nav2;
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

	#define CHEVAL_STEPS X(GO_DOWN) X(DRIVE) X(DRIVE_AND_STOW)
	enum class Cheval_steps{
		#define X(NAME) NAME,
		CHEVAL_STEPS
		#undef X
	};
	Cheval_steps cheval_step;
	
	Posedge_toggle learn;

	Checked_array<Panel_output,Panel_outputs::PANEL_OUTPUTS> main_panel_output;
	
	Tilt_presets tilt_presets;
	Log log;

	Toplevel::Goal teleop(Robot_inputs const&,Joystick_data const&,Joystick_data const&,Panel const&,Toplevel::Status_detail&,
		Tilt::Goal,
		Tilt::Goal,
		Tilt::Goal,
		Tilt::Goal,
		Tilt::Goal
	);

	Main();
	Robot_outputs operator()(Robot_inputs,std::ostream& = std::cerr);
};

std::ostream& operator<<(std::ostream&,Main::Mode);

bool operator==(Main const&,Main const&);
bool operator!=(Main const&,Main const&);
std::ostream& operator<<(std::ostream&,Main const&);

#endif
