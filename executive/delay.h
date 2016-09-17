#ifndef DELAY_H
#define DELAY_H

#include "executive.h"

struct Delay: public Executive_impl<Delay>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Delay const&)const;
};

#endif
