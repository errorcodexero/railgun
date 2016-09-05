#ifndef BARRELRACER_H
#define BARRELRACER_H

#include "mode.h"

struct Auto_br_straightaway : public Mode{
	unsigned int br_step;
	Countdown_timer in_br_range;

	Auto_br_straightaway(unsigned int bs):br_step(bs){}
	
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_br_initialturn : public Mode{
	unsigned int br_step;

	Auto_br_initialturn(unsigned int bs):br_step(bs){}

	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_br_side : public Mode{
	unsigned int br_step;

	Auto_br_side(unsigned int bs):br_step(bs){}
	
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_br_sideturn : public Mode{
	unsigned int br_step;

	Auto_br_sideturn(unsigned int bs):br_step(bs){}

	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

struct Auto_br_endturn : public Mode{
	unsigned int br_step;

	Auto_br_endturn(unsigned int bs):br_step(bs){}

	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
