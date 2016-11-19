#ifndef BARRELRACER_H
#define BARRELRACER_H

#include "executive.h"
#include "../util/motion_profile.h"
#include "../util/quick.h"

#define BARRELRACER_ITEMS(X)\
	X(unsigned,br_step)\
	X(Motion_profile,motion_profile)\
	X(SINGLE_ARG(std::pair<int,int>),initial_encoders)

struct Auto_br_straightaway : public Executive_impl<Auto_br_straightaway>{
	#define BARRELRACER_STRAIGHTAWAY_ITEMS(X)\
		X(Countdown_timer,in_br_range)\
		BARRELRACER_ITEMS(X)
	STRUCT_MEMBERS(BARRELRACER_STRAIGHTAWAY_ITEMS)
	
	Auto_br_straightaway(unsigned br_step1, std::pair<int,int> initial_encoders1):br_step(br_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_br_straightaway,BARRELRACER_STRAIGHTAWAY_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_straightaway const&)const;
};

struct Auto_br_initialturn : public Executive_impl<Auto_br_initialturn>{
	STRUCT_MEMBERS(BARRELRACER_ITEMS)
	
	Auto_br_initialturn(unsigned br_step1, std::pair<int,int> initial_encoders1):br_step(br_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_br_initialturn,BARRELRACER_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_initialturn const&)const;
};

struct Auto_br_side : public Executive_impl<Auto_br_side>{
	STRUCT_MEMBERS(BARRELRACER_ITEMS)
	
	Auto_br_side(unsigned br_step1, std::pair<int,int> initial_encoders1):br_step(br_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_br_side,BARRELRACER_ITEMS)
	
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_side const&)const;
};

struct Auto_br_sideturn : public Executive_impl<Auto_br_sideturn>{
	STRUCT_MEMBERS(BARRELRACER_ITEMS)
	
	Auto_br_sideturn(unsigned br_step1, std::pair<int,int> initial_encoders1):br_step(br_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_br_sideturn,BARRELRACER_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_sideturn const&)const;
};

struct Auto_br_endturn : public Executive_impl<Auto_br_endturn>{
	STRUCT_MEMBERS(BARRELRACER_ITEMS)
	
	Auto_br_endturn(unsigned br_step1, std::pair<int,int> initial_encoders1):br_step(br_step1),initial_encoders(initial_encoders1){}
	IMPL_STRUCT(Auto_br_endturn,BARRELRACER_ITEMS)

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_br_endturn const&)const;
};

#endif
