#ifndef BARRELRACER_H
#define BARRELRACER_H

#include "mode.h"
#include "../util/motion_profile.h"

struct Auto_br_straightaway : public Mode_impl<Auto_br_straightaway>{
	unsigned int br_step;
	Countdown_timer in_br_range;
	Motion_profile motion_profile;

	Auto_br_straightaway(unsigned int bs):br_step(bs){}
	Auto_br_straightaway(unsigned int bs,Motion_profile mp):br_step(bs),motion_profile(mp){}

	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_straightaway const&)const;
};

struct Auto_br_initialturn : public Mode_impl<Auto_br_initialturn>{
	unsigned int br_step;
	Motion_profile motion_profile;

	Auto_br_initialturn(unsigned int bs,Motion_profile mp):br_step(bs),motion_profile(mp){}

	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_initialturn const&)const;
};

struct Auto_br_side : public Mode_impl<Auto_br_side>{
	unsigned int br_step;
	Motion_profile motion_profile;

	Auto_br_side(unsigned int bs,Motion_profile mp):br_step(bs),motion_profile(mp){}
	
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_side const&)const;
};

struct Auto_br_sideturn : public Mode_impl<Auto_br_sideturn>{
	unsigned int br_step;
	Motion_profile motion_profile;

	Auto_br_sideturn(unsigned int bs,Motion_profile mp):br_step(bs),motion_profile(mp){}

	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_sideturn const&)const;
};

struct Auto_br_endturn : public Mode_impl<Auto_br_endturn>{
	unsigned int br_step;
	Motion_profile motion_profile;

	Auto_br_endturn(unsigned int bs,Motion_profile mp):br_step(bs),motion_profile(mp){}

	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_endturn const&)const;
};

#endif
