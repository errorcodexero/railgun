#ifndef AUTO_STATICTWO_H
#define AUTO_STATICTWO_H

#include "executive.h"

struct Auto_statictwo: public Executive_impl<Auto_statictwo>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_statictwo const&)const;
};

#endif
