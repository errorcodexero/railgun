#include "barrelracer.h"
#include "teleop.h"

using namespace std;

Mode Auto_br_straightaway::next_mode(Next_mode_info info){
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.first-initial_encoders.first,info.status.drive.ticks.second-initial_encoders.second);
	if(!info.autonomous) return Mode{Teleop()};
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
		return Mode{Teleop()};
		//return make_unique<Auto_br_initialturn>();//TODO 
	}
	return Mode{Auto_br_straightaway(br_step, info.status.drive.ticks)};
}

Toplevel::Goal Auto_br_straightaway::run(Run_info info){
	Toplevel::Goal goals;
	double power=-motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.first));
	goals.drive.left=power;
	goals.drive.right=power;
	return {};
}

bool Auto_br_straightaway::operator==(Auto_br_straightaway const& a)const{
	return br_step==a.br_step;
}

Mode Auto_br_initialturn::next_mode(Next_mode_info info){
	return Mode{Auto_br_initialturn(br_step, info.status.drive.ticks)};
}

Toplevel::Goal Auto_br_initialturn::run(Run_info){
	return {};
}

bool Auto_br_initialturn::operator==(Auto_br_initialturn const& a)const{
	return br_step==a.br_step;
}

Mode Auto_br_side::next_mode(Next_mode_info info){
	return Mode{Auto_br_side(br_step, info.status.drive.ticks)};
}

Toplevel::Goal Auto_br_side::run(Run_info){
	return {};
}

bool Auto_br_side::operator==(Auto_br_side const& a)const{
	return br_step==a.br_step;
}

Mode Auto_br_sideturn::next_mode(Next_mode_info info){
	return Mode{Auto_br_sideturn(br_step, info.status.drive.ticks)};
}

Toplevel::Goal Auto_br_sideturn::run(Run_info){
	return {};
}

bool Auto_br_sideturn::operator==(Auto_br_sideturn const& a)const{
	return br_step==a.br_step;
}

Mode Auto_br_endturn::next_mode(Next_mode_info info){
	return Mode{Auto_br_endturn(br_step,info.status.drive.ticks)};
}

Toplevel::Goal Auto_br_endturn::run(Run_info){
	return {};
}

bool Auto_br_endturn::operator==(Auto_br_endturn const& a)const{
	return br_step==a.br_step;
}

#ifdef BARRELRACER_TEST
int main(){
	#define STEPS \
		X(Auto_br_straightaway) \
		X(Auto_br_initialturn) \
		X(Auto_br_side)	\
		X(Auto_br_sideturn) \
		X(Auto_br_endturn)

	#define X(NAME) { NAME a(0,std::make_pair(0,0)); test_mode(a); }
	STEPS
	#undef X
}
#endif 
