#ifndef AUTO_TEST_H
#define AUTO_TEST_H

#include "mode.h"

struct Auto_test: public Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
