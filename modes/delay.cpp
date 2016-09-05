#include "delay.h"

#include "teleop.h" 
#include "auto_null.h"
#include "auto_reach.h"
#include "auto_statictwo.h"
#include "auto_static.h"
#include "portcullis.h"
#include "cheval.h"
#include "low_bar_low_score.h"
#include "low_bar_wall_low_score.h"
#include "low_bar_wall_high_score.h"
#include "barrelracer.h"

using namespace std;

unique_ptr<Mode> get_auto1(Panel const& panel){
	if (panel.in_use) {
		switch(panel.auto_mode){ 
			case Panel::Auto_mode::NOTHING:
				return make_unique<Auto_null>();
			case Panel::Auto_mode::REACH:
				return make_unique<Auto_reach>();
			case Panel::Auto_mode::STATICS:
				return make_unique<Auto_statictwo>();
			case Panel::Auto_mode::STATICF:
				return make_unique<Auto_static>();
			case Panel::Auto_mode::PORTCULLIS:
				return make_unique<Auto_portcullis>();
			case Panel::Auto_mode::CHEVAL:
				return make_unique<Auto_cheval_pos>();
			case Panel::Auto_mode::LBLS:
				return make_unique<Auto_lbls_cross_lb>();
			case Panel::Auto_mode::LBWLS:	
				return make_unique<Auto_lbwls_wall>();
			case Panel::Auto_mode::LBWHS:
				return make_unique<Auto_lbwhs_wall>();
			case Panel::Auto_mode::S:
				return make_unique<Auto_lbwhs_prep>();
			case Panel::Auto_mode::BR:
				assert(0);//return make_unique<Auto_br_straightaway>();
			default: assert(0);
		}
	}
	return make_unique<Delay>();
}

unique_ptr<Mode> Delay::next_mode(Next_mode_info info){
	if(!info.autonomous) return make_unique<Teleop>();
	if(info.since_switch > (info.panel.speed_dial+1)*5 || info.since_switch > 8) return get_auto1(info.panel);
	return make_unique<Delay>();
}

Toplevel::Goal Delay::run(Run_info){
	
	return {};
}

#ifdef DELAY_TEST
int main(){
	
}
#endif 
