#ifndef TELEOP_H
#define TELEOP_H

#include "mode.h"
#include "../util/posedge_trigger_debounce.h"
#include "../util/posedge_toggle.h"

/*struct Shooter_constants{
	PID_values pid;
	float ground,climbed;

	Shooter_constants();
};

struct Tilt_presets{
	double top, level, low, cheval;//angles (in degrees) that it will go to when set to the tilt goals
	Tilt_presets();
};*/

#define SHOOT_STEPS X(SPEED_UP) X(SHOOT)
enum class Shoot_steps{
	#define X(NAME) NAME,
	SHOOT_STEPS
	#undef X
};

#define COLLECTOR_MODES X(CHEVAL) X(NOTHING) X(COLLECT) X(STOW) X(SHOOT_HIGH) X(REFLECT) X(SHOOT_LOW) X(LOW) X(DRAWBRIDGE)
enum class Collector_mode{
	#define X(name) name,
	COLLECTOR_MODES
	#undef X
};

enum Nudges{FORWARD,BACKWARD,CLOCKWISE,COUNTERCLOCKWISE,NUDGES};
struct Nudge{
	Posedge_trigger trigger;
	Countdown_timer timer;
};

struct Teleop : Mode {
	//Shooter_constants shooter_constants;
	//Tilt_presets tilt_presets;
	Shoot_steps shoot_step;
	Countdown_timer shoot_high_timer;
	Collector_mode collector_mode;
	array<Nudge,NUDGES> nudges;
	//Posedge_toggle controller_auto;

	Shooter::Goal shoot_action(Panel::Shooter_mode,double,bool)const;
	void shooter_protocol(Toplevel::Status_detail const&,const bool,const Time,Toplevel::Goal&,bool,Panel::Shooter_mode,double);

	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
