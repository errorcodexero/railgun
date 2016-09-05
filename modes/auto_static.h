#ifndef AUTO_STATIC_H
#define AUTO_STATIC_H

#include "mode.h"

struct Auto_static: public Mode_impl<Auto_static>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
