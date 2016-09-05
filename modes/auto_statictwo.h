#ifndef AUTO_STATICTWO_H
#define AUTO_STATICTWO_H

#include "mode.h"

struct Auto_statictwo: public Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
