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
	#define MODES X(TELEOP) X(AUTO_MOVE) X(AUTO_NAV) X(AUTO_NAV_RUN) X(AUTO_NAV_DAMAGE_DRIVE) X(AUTO_NAV_DAMAGE_MANIPULATOR)
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
	/*struct oiload{
		point ptone;
		point pttwo;
		direction dirone;
		direction dirtwo;
	};
	*/
	struct navinput{
		point navpt;
		direction navdir;
	};
	
	unsigned int navindex;
	std::vector<NavS> NavV;
		
	vector<NavS> loadnav();	

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
	#define COLLECTOR_MODES X(NOTHING) X(COLLECT) X(STOW) X(EJECT) X(REFLECT) X(TERRAIN) X(LOW_BAR)
	enum class Collector_mode{
		#define X(name) name,
		COLLECTOR_MODES
		#undef X
	};
	Collector_mode collector_mode;
		
	enum Panel_outputs{SHOOTER_PREPPED, BOULDER, PANEL_OUTPUTS};
	enum Panel_output_ports{PSHOOTER_PREPPED=6, PBOULDER=10};

	Checked_array<Panel_output,PANEL_OUTPUTS> main_panel_output;
		
	Toplevel::Goal teleop(Robot_inputs const&,Joystick_data const&,Joystick_data const&,Panel const&,Toplevel::Status_detail&);
	Main();
	Robot_outputs operator()(Robot_inputs,std::ostream& = std::cerr);
};

std::ostream& operator<<(std::ostream&,Main::Mode);

bool operator==(Main,Main);
bool operator!=(Main,Main);
std::ostream& operator<<(std::ostream&,Main);

#endif
