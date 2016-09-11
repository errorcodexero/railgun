#ifndef DELAY_H
#define DELAY_H

#include "mode.h"

struct Delay: public Mode_impl<Delay>{
	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Delay const&)const;
};

#endif
