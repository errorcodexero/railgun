#ifndef CHEVAL_H
#define CHEVAL_H

#include "mode.h"

struct Auto_cheval_pos: public Mode_impl<Auto_cheval_pos>{
	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_pos const&)const;
};

struct Auto_cheval_wait: public Mode_impl<Auto_cheval_wait>{
	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_wait const&)const;
};

struct Auto_cheval_drop: public Mode_impl<Auto_cheval_drop>{
	bool topready;

	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_drop const&)const;
	Auto_cheval_drop(){ topready = false; }
};

struct Auto_cheval_drive: public Mode_impl<Auto_cheval_drive>{
	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_drive const&)const;
};

struct Auto_cheval_stow: public Mode_impl<Auto_cheval_stow>{
	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_stow const&)const;
};

#endif
