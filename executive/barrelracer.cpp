#include "barrelracer.h"
#include "teleop.h"

using namespace std;

Executive Auto_br_straightaway::next_mode(Next_mode_info info){
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.first-initial_encoders.first,info.status.drive.ticks.second-initial_encoders.second);
	if(!info.autonomous) return Executive{Teleop()};
	const double TARGET_DISTANCE = 5.0*12.0;//inches
	const double TOLERANCE = 6.0;//inches
	motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.first<<"   "<<ticks_to_inches(encoder_differences.first)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.first) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.first) <= TARGET_DISTANCE+TOLERANCE){
		in_br_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_br_range.set(2.0);
	}
	if(in_br_range.done()){
		//set_initial_encoders=false;
		return Executive{Teleop()};
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Executive{Auto_br_straightaway(CONSTRUCT_STRUCT_PARAMS(BARRELRACER_STRAIGHTAWAY_ITEMS))};
}

Toplevel::Goal Auto_br_straightaway::run(Run_info info){
	Toplevel::Goal goals;
	double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.first));
	goals.drive.left=power;
	goals.drive.right=power;
	return goals;
}

Executive Auto_br_initialturn::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	return Executive{Auto_br_initialturn(CONSTRUCT_STRUCT_PARAMS(BARRELRACER_ITEMS))};
}

Toplevel::Goal Auto_br_initialturn::run(Run_info){
	return {};
}

Executive Auto_br_side::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	return Executive{Auto_br_side(CONSTRUCT_STRUCT_PARAMS(BARRELRACER_ITEMS))};
}

Toplevel::Goal Auto_br_side::run(Run_info){
	return {};
}

Executive Auto_br_sideturn::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	return Executive{Auto_br_sideturn(CONSTRUCT_STRUCT_PARAMS(BARRELRACER_ITEMS))};
}

Toplevel::Goal Auto_br_sideturn::run(Run_info){
	return {};
}

Executive Auto_br_endturn::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	return Executive{Auto_br_endturn(CONSTRUCT_STRUCT_PARAMS(BARRELRACER_ITEMS))};
}

Toplevel::Goal Auto_br_endturn::run(Run_info){
	return {};
}

#define STEPS \
	X(straightaway) \
	X(initialturn) \
	X(side)	\
	X(sideturn) \
	X(endturn)

#define X(NAME) bool Auto_br_##NAME::operator==(Auto_br_##NAME const& a)const{ return br_step==a.br_step; }
STEPS
#undef X

#ifdef BARRELRACER_TEST
#include "test.h"
int main(){
	#define X(NAME) { Auto_br_##NAME a(0,std::make_pair(0,0)); test_executive(a); }
	STEPS
	#undef X
}
#endif
