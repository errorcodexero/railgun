#ifndef DELAY_H
#define DELAY_H

#include "mode.h"

struct Delay: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
