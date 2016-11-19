#include "low_bar_wall_high_score.h"

#include "teleop.h"

using namespace std;

using Mode=Executive;

Mode Auto_lbwhs_wall::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_wall()};
}

Toplevel::Goal Auto_lbwhs_wall::run(Run_info){
	return {};
}

Mode Auto_lbwhs_mup::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_mup()};
}

Toplevel::Goal Auto_lbwhs_mup::run(Run_info){
	return {};
}

Mode Auto_lbwhs_rotate::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_rotate()};
}

Toplevel::Goal Auto_lbwhs_rotate::run(Run_info){
	return {};
}

Mode Auto_lbwhs_tower::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_tower()};
}

Toplevel::Goal Auto_lbwhs_tower::run(Run_info){
	return {};
}

Mode Auto_lbwhs_eject::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_eject()};
}

Toplevel::Goal Auto_lbwhs_eject::run(Run_info){
	return {};
}

Mode Auto_lbwhs_back::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_back()};
}

Toplevel::Goal Auto_lbwhs_back::run(Run_info){
	return {};
}

Mode Auto_lbwhs_c::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_c()};
}

Toplevel::Goal Auto_lbwhs_c::run(Run_info){
	return {};
}

Mode Auto_lbwhs_br::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_br()};
}

Toplevel::Goal Auto_lbwhs_br::run(Run_info){
	return {};
}

Mode Auto_lbwhs_prep::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_prep()};
}

Toplevel::Goal Auto_lbwhs_prep::run(Run_info){
	return {};
}

Mode Auto_lbwhs_bp::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	return Mode{Auto_lbwhs_bp()};
}

Toplevel::Goal Auto_lbwhs_bp::run(Run_info){
	return {};
}

#define STEPS X(wall) X(mup) X(rotate) X(tower) X(eject) X(back) X(c) X(br) X(prep) X(bp)

#define X(NAME) bool Auto_lbwhs_##NAME::operator==(Auto_lbwhs_##NAME const&)const{ return 1; }
STEPS
#undef X

#ifdef LOW_BAR_WALL_HIGH_SCORE_TEST
#include "test.h"
int main(){
	#define X(NAME) { Auto_lbwhs_##NAME a; test_executive(a); }
	STEPS
	#undef X
}
#endif 
