#ifndef CHEVAL_H
#define CHEVAL_H

#include "mode.h"

struct Auto_cheval_pos: public Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_cheval_wait: public Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
