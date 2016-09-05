#ifndef AUTO_TEST_H
#define AUTO_TEST_H

#include "mode.h"

struct Auto_test: public Mode_impl<Auto_test>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_test const&)const;
};

#endif
