#ifndef AUTO_STOP_H
#define AUTO_STOP_H

#include "executive.h"

struct Auto_stop: public Executive_impl<Auto_stop>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_stop const&)const;
};

#endif
