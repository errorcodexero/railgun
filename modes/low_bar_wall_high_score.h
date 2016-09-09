#ifndef LOW_BAR_WALL_HIGH_SCORE_H
#define LOW_BAR_WALL_HIGH_SCORE_H

#include "mode.h"

struct Auto_lbwhs_wall: public Mode_impl<Auto_lbwhs_wall>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_wall const&)const;
};

struct Auto_lbwhs_mup: public Mode_impl<Auto_lbwhs_mup>{
	bool topready;
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_mup const&)const;
};

struct Auto_lbwhs_rotate: public Mode_impl<Auto_lbwhs_rotate>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_rotate const&)const;
};

struct Auto_lbwhs_tower: public Mode_impl<Auto_lbwhs_tower>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_tower const&)const;
};

struct Auto_lbwhs_eject: public Mode_impl<Auto_lbwhs_eject>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_eject const&)const;
};

struct Auto_lbwhs_back: public Mode_impl<Auto_lbwhs_back>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_back const&)const;
};

struct Auto_lbwhs_c: public Mode_impl<Auto_lbwhs_c>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_c const&)const;
};

struct Auto_lbwhs_br: public Mode_impl<Auto_lbwhs_br>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_br const&)const;
};

struct Auto_lbwhs_prep: public Mode_impl<Auto_lbwhs_prep>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_prep const&)const;
};

struct Auto_lbwhs_bp: public Mode_impl<Auto_lbwhs_bp>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_bp const&)const;
};

#endif
