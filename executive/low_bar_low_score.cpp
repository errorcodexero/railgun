#include "low_bar_low_score.h"
#include "teleop.h"
#include "../control/tilt_presets.h"

using namespace std;

using Mode=Executive;

Mode Auto_lbls_cross_lb::next_mode(Next_mode_info info){	
	if(!info.autonomous) return Mode{Teleop()};
	pair<int,int> encoder_differences=make_pair(info.status.drive.ticks.first-initial_encoders.first,info.status.drive.ticks.second-initial_encoders.second);
	if(encoder_differences.first >= 670) return Mode{Auto_lbls_cross_mu()};
	// 100 ticks per 1 revolution   |    8in wheel   |     167 in for first run     |     cir:25.12     |    100 ticks / 25 in      |      4 ticks / 1 in     |      668 ticks / 167 in.
	return Mode{Auto_lbls_cross_lb(initial_encoders)};
}

Toplevel::Goal Auto_lbls_cross_lb::run(Run_info info){
	Tilt_presets tilt_presets=read_tilt_presets();
	Toplevel::Goal goals;

	goals.collector.front = Front::Goal::OFF;
	goals.collector.sides = Sides::Goal::OFF;
	goals.collector.tilt = Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.low));
	
	if(ready(info.toplevel_status.collector.tilt.angle,goals.collector.tilt)){	
		goals.drive.left = -.5;
		goals.drive.right = -.5;
	}
	return goals;
}

Mode Auto_lbls_cross_mu::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(topready) return Mode{Auto_lbls_score_seek()};
	return Mode{Auto_lbls_cross_mu()};
}

Toplevel::Goal Auto_lbls_cross_mu::run(Run_info info){
	Toplevel::Goal goals;
	Tilt_presets tilt_presets=read_tilt_presets();
	goals.collector.tilt = Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));
	topready = ready(info.toplevel_status.collector.tilt.angle, goals.collector.tilt);
	return goals;
}


Mode Auto_lbls_score_seek::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > .76) return Mode{Auto_lbls_score_locate()};
	return Mode{Auto_lbls_score_seek()};
}

Toplevel::Goal Auto_lbls_score_seek::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left = -.5;
	goals.drive.right = .5;
	return goals;
}


Mode Auto_lbls_score_locate::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > 1) return Mode{Auto_lbls_score_cd()};
	return Mode{Auto_lbls_score_locate()};
}

Toplevel::Goal Auto_lbls_score_locate::run(Run_info){
	//do nothing
	return {};
}


Mode Auto_lbls_score_cd::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > 2.8) return Mode{Auto_lbls_score_eject()};
	return Mode{Auto_lbls_score_cd()};
}

Toplevel::Goal Auto_lbls_score_cd::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left = -.5;
	goals.drive.right = -.5;
	return goals;
}


Mode Auto_lbls_score_eject::next_mode(Next_mode_info info){
	if(!info.autonomous || info.since_switch > 1) return Mode{Teleop()};
	return Mode{Auto_lbls_score_eject()};
}

Toplevel::Goal Auto_lbls_score_eject::run(Run_info){
	Toplevel::Goal goals;
	Tilt_presets tilt_presets=read_tilt_presets();
	goals.collector = {Front::Goal::OUT,Sides::Goal::OFF, Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top))};
	return goals;
}
	#define STEPS\
		X(cross_lb)\
		X(cross_mu)\
		X(score_seek)\
		X(score_locate)\
		X(score_cd)\
		X(score_eject)

#define X(NAME) bool Auto_lbls_##NAME::operator==(Auto_lbls_##NAME const&)const{ return 1; }
STEPS
#undef X

#ifdef LOW_BAR_LOW_SCORE_TEST
#include "test.h"
int main(){

	#define X(NAME) { Auto_lbls_##NAME a; test_executive(a); }
	STEPS
	#undef X
}
#endif 
