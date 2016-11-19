#ifndef AUTO_REACH_H
#define AUTO_REACH_H

#include "executive.h"

struct Auto_reach: public Executive_impl<Auto_reach>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_reach const&)const;
};

#endif
