#ifndef LOW_BAR_WALL_HIGH_SCORE_H
#define LOW_BAR_WALL_HIGH_SCORE_H

#include "mode.h"

struct Auto_lbwhs_wall: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_mup: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_rotate: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_tower: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_eject: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_back: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_c: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_br: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_prep: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwhs_bp: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
