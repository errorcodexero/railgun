#ifndef TOPLEVEL_MODE_H
#define TOPLEVEL_MODE_H

#include "toplevel.h"

namespace Toplevel_mode{
	//all this mode stuff really belongs elsewhere
	enum Mode{
		DRIVE_WO_BALL,DRIVE_W_BALL,
		COLLECT,
		SHOOT_HIGH_PREP,SHOOT_HIGH,
		TRUSS_TOSS_PREP,TRUSS_TOSS,
		//PASS_PREP,PASS,
		EJECT_PREP,EJECT,
		AUTO_SHOT_PREP, AUTO_SHOT,
		CATCH, //SHOOT_LOW
		COLLECT_SPIN_UP,
		SHOOT_HIGH_PREP_NO_PUMP,
		SHOOT_HIGH_NO_PUMP/*,
		AUTO_SHOT,
		AUTO_SHOT_PREP*/
	};
	std::ostream& operator<<(std::ostream& o,Mode);

	Toplevel::Goal subgoals(Mode);
}

#endif
