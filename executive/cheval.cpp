#include "cheval.h"

#include "teleop.h"
#include "../control/tilt_presets.h"
#include "../control/tilt.h"

using namespace std;

Executive Auto_cheval_pos::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	if(info.since_switch > 1.8) return Executive{Auto_cheval_wait()};
	return Executive{Auto_cheval_pos()};
}

Toplevel::Goal Auto_cheval_pos::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=-.25;
	goals.drive.right=-.25;
	return goals;
}

bool Auto_cheval_pos::operator==(Auto_cheval_pos const&)const{
	return 1;
}

Executive Auto_cheval_wait::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	if(info.since_switch > 3) return Executive{Auto_cheval_drop()};
	return Executive{Auto_cheval_wait()};
}

Toplevel::Goal Auto_cheval_wait::run(Run_info){
	//do nothing
	return {};
}

bool Auto_cheval_wait::operator==(Auto_cheval_wait const&)const{
	return 1;
}

Executive Auto_cheval_drop::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	if(topready) return Executive{Auto_cheval_drive()};
	return Executive{Auto_cheval_drop()};
}

Toplevel::Goal Auto_cheval_drop::run(Run_info info){
	Toplevel::Goal goals;
	Tilt_presets tilt_presets=read_tilt_presets();
	goals.collector.tilt = Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.cheval));
	topready=ready(info.toplevel_status.collector.tilt.angle,goals.collector.tilt);
	return goals;
}

bool Auto_cheval_drop::operator==(Auto_cheval_drop const&)const{
	return 1;
}

Executive Auto_cheval_drive::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	if(info.since_switch > .45) return Executive{Auto_cheval_stow()};
	return Executive{Auto_cheval_drive()};
}

Toplevel::Goal Auto_cheval_drive::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=-.5;
	goals.drive.right=.5;
	return goals;
}

bool Auto_cheval_drive::operator==(Auto_cheval_drive const&)const{
	return 1;
}

Executive Auto_cheval_stow::next_mode(Next_mode_info info){
	if(!info.autonomous || info.since_switch > 2.5) return Executive{Teleop()};
	return Executive{Auto_cheval_stow()};
}

Toplevel::Goal Auto_cheval_stow::run(Run_info){
	Tilt_presets tilt_presets=read_tilt_presets();
	Toplevel::Goal goals;
	goals.drive.left=-.5;
	goals.drive.right=-.5;
	goals.collector.tilt=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));
	return goals;
}

bool Auto_cheval_stow::operator==(Auto_cheval_stow const&)const{
	return 1;
}


#ifdef CHEVAL_TEST
#include "test.h"
int main(){
	{
		Auto_cheval_pos a;
		test_executive(a);
	}
	{
		Auto_cheval_wait a;
		test_executive(a);
	}
}
#endif 
