#ifndef PORTCULLIS_H
#define PORTCULLIS_H

#include "mode.h"

struct Portcullis: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
