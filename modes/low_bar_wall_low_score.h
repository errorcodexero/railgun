#ifndef LOW_BAR_WALL_LOW_SCORE_H
#define LOW_BAR_WALL_LOW_SCORE_H

#include "mode.h"

struct Auto_lbwls_wall: public Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_mup: Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_rotate: Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_tower: Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_eject: Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_back: Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_c: Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_br: Mode{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
