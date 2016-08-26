#ifndef AUTO_STOP_H
#define AUTO_STOP_H

#include "mode.h"

struct Auto_stop: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
