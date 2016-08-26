#ifndef CHEVAL_H
#define CHEVAL_H

#include "mode.h"

struct Cheval: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
