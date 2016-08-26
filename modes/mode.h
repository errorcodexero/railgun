#ifndef MODE_H
#define MODE_H

#include <memory>
#include "../control/toplevel.h"
#include "../input/panel.h"
#include "../util/interface.h"

struct Next_mode_info {
	bool autonomous, autonomous_start;
	Toplevel::Status_detail const status;
	Time since_switch;
	Panel panel;
	bool const toplready;
	Robot_inputs const in;
	pair<int,int> initial_encoders;
};

struct Run_info {
	Robot_inputs const& in;
	Joystick_data const& main_joystick, gunner_joystick;
	Panel const& panel;
	Toplevel::Status_detail const& toplevel_status;
	Tilt::Goal level, low, top, cheval, drawbridge;
};

struct Mode {
	virtual unique_ptr<Mode> next_mode(Next_mode_info)=0;
	virtual Toplevel::Goal run(Run_info)=0;
};

#endif
