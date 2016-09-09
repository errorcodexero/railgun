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

struct Auto_cheval_drop: public Mode_impl<Auto_cheval_drop>{
	bool topready;
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_drop const&)const;
};

struct Auto_cheval_drive: public Mode_impl<Auto_cheval_drive>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_drive const&)const;
};

struct Auto_cheval_stow: public Mode_impl<Auto_cheval_stow>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_cheval_stow const&)const;
};

#endif
