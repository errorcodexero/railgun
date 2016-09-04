#ifndef TELEOP_H
#define TELEOP_H

#include "mode.h"
#include "../util/posedge_trigger_debounce.h"
#include "../util/posedge_toggle.h"

struct Teleop : Mode {
	/*
	struct Shooter_constants{
		PID_values pid;
		float ground,climbed;

		Shooter_constants();
	};
	Shooter_constants shooter_constants;

	Tilt_presets tilt_presets;

	#define SHOOT_STEPS X(SPEED_UP) X(SHOOT)
	enum class Shoot_steps{
		#define X(NAME) NAME,
		SHOOT_STEPS
		#undef X
	};	
	Shoot_steps shoot_step;
	
	Countdown_timer shoot_high_timer, shoot_low_timer, cheval_lift_timer, cheval_drive_timer;
	
	#define COLLECTOR_MODES X(CHEVAL) X(NOTHING) X(COLLECT) X(STOW) X(SHOOT_HIGH) X(REFLECT) X(SHOOT_LOW) X(LOW) X(DRAWBRIDGE)
	enum class Collector_mode{
		#define X(name) name,
		COLLECTOR_MODES
		#undef X
	};
	Collector_mode collector_mode;
	
	enum Nudges{FORWARD,BACKWARD,CLOCKWISE,COUNTERCLOCKWISE,NUDGES};
	struct Nudge{
		Posedge_trigger trigger;
		Countdown_timer timer;
	};
	array<Nudge,NUDGES> nudges;
	
	Posedge_toggle controller_auto;
	Posedge_trigger_debounce cheval_trigger;
	bool tilt_learn_mode;

	#define CHEVAL_STEPS X(GO_DOWN) X(DRIVE) X(DRIVE_AND_STOW)
	enum class Cheval_steps{
		#define X(NAME) NAME,
		CHEVAL_STEPS
		#undef X
	};
	Cheval_steps cheval_step;

	#define JOY_COLLECTOR_POS X(STOP) X(LOW) X(LEVEL)
	enum class Joy_collector_pos{
		#define X(name) name,
		JOY_COLLECTOR_POS
		#undef X
	};
	Joy_collector_pos joy_collector_pos;	

	Shooter::Goal shoot_action(Panel::Shooter_mode,double,bool)const;
	void shooter_protocol(Toplevel::Status_detail const&,const bool,const Time,Toplevel::Goal&,bool,Panel::Shooter_mode,double);
	*/

	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
