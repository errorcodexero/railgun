#include "barrelracer.h"

#include "teleop.h"

unique_ptr<Mode> Auto_br_straightaway::next_mode(Next_mode_info info){
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.first-info.initial_encoders.first,info.status.drive.ticks.second-info.initial_encoders.second);
	if(!info.autonomous) return make_unique<Teleop>();
	const double TARGET_DISTANCE = 5.0*12.0;//inches
	const double TOLERANCE = 6.0;//inches
	//motion_profile.set_goal(TARGET_DISTANCE);
	cout<<"\n"<<encoder_differences.first<<"   "<<ticks_to_inches(encoder_differences.first)<<"   "<<TARGET_DISTANCE<<"\n";
	if(ticks_to_inches(encoder_differences.first) >= TARGET_DISTANCE-TOLERANCE && ticks_to_inches(encoder_differences.first) <= TARGET_DISTANCE+TOLERANCE){
		in_br_range.update(info.in.now,info.in.robot_mode.enabled);
	}
	else{
		in_br_range.set(2.0);
	}
	if(in_br_range.done()){
		//set_initial_encoders=false;
		return make_unique<Teleop>();
		//return make_unique<Auto_br_initialturn>();
	}
	return make_unique<Auto_br_straightaway>(br_step);
}

Toplevel::Goal Auto_br_straightaway::run(Run_info){
	Toplevel::Goal goals;
	double power=0;//-info.motion_profile.target_speed(ticks_to_inches(info.toplevel_status.drive.ticks.first));
	goals.drive.left=power;
	goals.drive.right=power;
	return {};
}

unique_ptr<Mode> Auto_br_initialturn::next_mode(Next_mode_info){
	return make_unique<Auto_br_initialturn>(br_step);
}

Toplevel::Goal Auto_br_initialturn::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_br_side::next_mode(Next_mode_info){
	return make_unique<Auto_br_side>(br_step);
}

Toplevel::Goal Auto_br_side::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_br_sideturn::next_mode(Next_mode_info){
	return make_unique<Auto_br_sideturn>(br_step);
}

Toplevel::Goal Auto_br_sideturn::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_br_endturn::next_mode(Next_mode_info){
	return make_unique<Auto_br_endturn>(br_step);
}

Toplevel::Goal Auto_br_endturn::run(Run_info){
	return {};
}

#ifdef BARRELRACER_TEST
int main(){
	
}
#endif 
