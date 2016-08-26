#ifndef BARRELRACER_H
#define BARRELRACER_H

#include "mode.h"

struct Barrelracer: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
