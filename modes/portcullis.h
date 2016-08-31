#ifndef PORTCULLIS_H
#define PORTCULLIS_H

#include "mode.h"

struct Auto_portcullis: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_portcullis_done: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};
#endif
