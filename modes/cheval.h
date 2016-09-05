#ifndef CHEVAL_H
#define CHEVAL_H

#include "mode.h"

struct Auto_cheval_pos: public Mode_impl<Auto_cheval_pos>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_pos const&)const;
};

struct Auto_cheval_wait: public Mode_impl<Auto_cheval_wait>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_wait const&)const;
};

#endif
