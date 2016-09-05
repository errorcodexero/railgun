#ifndef AUTO_REACH_H
#define AUTO_REACH_H

#include "mode.h"

struct Auto_reach: public Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
