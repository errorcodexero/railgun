#ifndef AUTO_TEST_H
#define AUTO_TEST_H

#include "executive.h"

struct Auto_test: public Executive_impl<Auto_test>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_test const&)const;
};

#endif
