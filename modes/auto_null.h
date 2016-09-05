#ifndef AUTO_NULL_H
#define AUTO_NULL_H

#include "mode.h"

struct Auto_null: public Mode_impl<Auto_null>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_null const&)const;
};

#endif
