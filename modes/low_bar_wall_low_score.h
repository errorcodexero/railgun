#ifndef LOW_BAR_WALL_LOW_SCORE_H
#define LOW_BAR_WALL_LOW_SCORE_H

#include "mode.h"

struct Auto_lbwls_wall: public Mode_impl<Auto_lbwls_wall>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_mup: Mode_impl<Auto_lbwls_mup>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_rotate: Mode_impl<Auto_lbwls_rotate>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_tower: Mode_impl<Auto_lbwls_tower>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_eject: Mode_impl<Auto_lbwls_eject>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_back: Mode_impl<Auto_lbwls_back>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_c: Mode_impl<Auto_lbwls_c>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_lbwls_br: Mode_impl<Auto_lbwls_br>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
