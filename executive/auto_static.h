#ifndef AUTO_STATIC_H
#define AUTO_STATIC_H

#include "executive.h"

struct Auto_static: public Executive_impl<Auto_static>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_static const&)const;
};

#endif
