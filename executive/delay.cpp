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

using Mode=Executive;

Executive get_auto1(Next_mode_info info){
	if (info.panel.in_use) {
		switch(info.panel.auto_mode){ 
			case Panel::Auto_mode::NOTHING:
				return Executive{Auto_null()};
			case Panel::Auto_mode::REACH:
				return Executive{Auto_reach()};
			case Panel::Auto_mode::STATICS:
				return Executive{Auto_statictwo()};
			case Panel::Auto_mode::STATICF:
				return Executive{Auto_static()};
			case Panel::Auto_mode::PORTCULLIS:
				return Mode{Auto_portcullis()};
			case Panel::Auto_mode::CHEVAL:
				return Mode{Auto_cheval_pos()};
			case Panel::Auto_mode::LBLS:
				return Mode{Auto_lbls_cross_lb(info.status.drive.ticks)};
			case Panel::Auto_mode::LBWLS:	
				return Mode{Auto_lbwls_wall()};
			case Panel::Auto_mode::LBWHS:
				return Mode{Auto_lbwhs_wall()};
			case Panel::Auto_mode::S:
				return Mode{Auto_lbwhs_prep()};
			case Panel::Auto_mode::BR:
				//FIXME: For now, just choosing some number to put in.
				return Mode{Auto_br_straightaway(0, info.status.drive.ticks)};
			default: assert(0);
		}
	}
	return Mode{Delay()};
}

Mode Delay::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > (info.panel.speed_dial+1)*5 || info.since_switch > 8) return get_auto1(info);
	return Mode{Delay()};
}

Toplevel::Goal Delay::run(Run_info){
	
	return {};
}

bool Delay::operator==(Delay const&)const{ return 1; }

#ifdef DELAY_TEST
#include "test.h"
int main(){
	Delay a;
	test_executive(a);
}
#endif 
