#include "low_bar_low_score.h"
#include "teleop.h"
#include "../control/tilt_presets.h"

using namespace std;

unique_ptr<Mode> Auto_lbls_cross_lb::next_mode(Next_mode_info info){
	
	pair<int,int> current_encoders=info.status.drive.ticks;//{encoderconv(in.digital_io.encoder[0]),encoderconv(in.digital_io.encoder[1])};//first is left, second is right
	pair<int,int> encoder_differences=make_pair(current_encoders.first-info.initial_encoders.first,current_encoders.second-info.initial_encoders.second);
	if(!info.autonomous) return make_unique<Teleop>();
	if(encoder_differences.first >= 670) return make_unique<Auto_lbls_cross_mu>();
// 100 ticks per 1 revalition| 8in wheal| 167 in for first run| cir:25.12| 100 ticks / 25 in| 4 ticks / 1 in| 668 ticks / 167 in.
	return make_unique<Auto_lbls_cross_lb>();
}

Toplevel::Goal Auto_lbls_cross_lb::run(Run_info info){
	Tilt_presets tilt_presets;
	Toplevel::Goal goals;
	Tilt::Goal low=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.low));

	goals.collector.front=Front::Goal::OFF;
	goals.collector.sides=Sides::Goal::OFF;
	goals.collector.tilt=low;
	
		if(ready(info.toplevel_status.collector.tilt.angle,goals.collector.tilt)){	
			goals.drive.left =.5;
			goals.drive.right =.5;
		}
	return {};
}

unique_ptr<Mode> Auto_lbls_cross_mu::next_mode(Next_mode_info info){
	if(!info.autonomous) return make_unique<Teleop>();
	if(info.toplready) return make_unique<Auto_lbls_score_seek>();
	return make_unique<Auto_lbls_cross_mu>();
}

Toplevel::Goal Auto_lbls_cross_mu::run(Run_info){
	return {};
}


unique_ptr<Mode> Auto_lbls_score_seek::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_score_seek>();
}

Toplevel::Goal Auto_lbls_score_seek::run(Run_info){
	return {};
}


unique_ptr<Mode> Auto_lbls_score_locate::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_score_locate>();
}

Toplevel::Goal Auto_lbls_score_locate::run(Run_info){
	return {};
}


unique_ptr<Mode> Auto_lbls_score_cd::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_score_cd>();
}

Toplevel::Goal Auto_lbls_score_cd::run(Run_info){
	return {};
}


unique_ptr<Mode> Auto_lbls_score_eject::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_score_eject>();
}

Toplevel::Goal Auto_lbls_score_eject::run(Run_info){
	return {};
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
int main(){

	#define X(NAME) { Auto_lbls_##NAME a; test_mode(a); }
	STEPS
	#undef X
}
#endif 
