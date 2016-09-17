#ifndef AUTO_NULL_H
#define AUTO_NULL_H

#include "executive.h"

struct Auto_null: public Executive_impl<Auto_null>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_null const&)const;
};

#endif
