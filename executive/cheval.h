#ifndef CHEVAL_H
#define CHEVAL_H

#include "executive.h"

struct Auto_cheval_pos: public Executive_impl<Auto_cheval_pos>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_pos const&)const;
};

struct Auto_cheval_wait: public Executive_impl<Auto_cheval_wait>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_wait const&)const;
};

struct Auto_cheval_drop: public Executive_impl<Auto_cheval_drop>{
	bool topready;

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_drop const&)const;
	Auto_cheval_drop(){ topready = false; }
};

struct Auto_cheval_drive: public Executive_impl<Auto_cheval_drive>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_drive const&)const;
};

struct Auto_cheval_stow: public Executive_impl<Auto_cheval_stow>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_stow const&)const;
};

#endif
