#ifndef LOW_BAR_WALL_HIGH_SCORE_H
#define LOW_BAR_WALL_HIGH_SCORE_H

#include "executive.h"

struct Auto_lbwhs_wall: public Mode_impl<Auto_lbwhs_wall>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_wall const&)const;
};

struct Auto_lbwhs_mup: public Mode_impl<Auto_lbwhs_mup>{
	bool topready;
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_mup const&)const;
	Auto_lbwhs_mup(){ topready = false; }
};

struct Auto_lbwhs_rotate: public Mode_impl<Auto_lbwhs_rotate>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_rotate const&)const;
};

struct Auto_lbwhs_tower: public Mode_impl<Auto_lbwhs_tower>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_tower const&)const;
};

struct Auto_lbwhs_eject: public Mode_impl<Auto_lbwhs_eject>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_eject const&)const;
};

struct Auto_lbwhs_back: public Mode_impl<Auto_lbwhs_back>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_back const&)const;
};

struct Auto_lbwhs_c: public Mode_impl<Auto_lbwhs_c>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_c const&)const;
};

struct Auto_lbwhs_br: public Mode_impl<Auto_lbwhs_br>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_br const&)const;
};

struct Auto_lbwhs_prep: public Mode_impl<Auto_lbwhs_prep>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_prep const&)const;
};

struct Auto_lbwhs_bp: public Mode_impl<Auto_lbwhs_bp>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbwhs_bp const&)const;
};

#endif
