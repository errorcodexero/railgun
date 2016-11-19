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
		switch(info.panel.auto_switch.get()){ 
			case 0:
				return Executive{Auto_null()};
			case 1: 
				return Executive{Auto_reach()};
			case 2:
				return Executive{Auto_statictwo()};
			case 3: 
				return Executive{Auto_static()};
			case 4:
				return Mode{Auto_portcullis()};
			case 5:
				return Mode{Auto_cheval_pos()};
			case 6:
				return Mode{Auto_lbls_cross_lb(info.status.drive.ticks)};
			case 7:	
				return Mode{Auto_lbwls_wall()};
			case 8:
				return Mode{Auto_lbwhs_wall()};
			case 9:
				//FIXME: For now, just choosing some number to put in.
				return Mode{Auto_br_straightaway(0, info.status.drive.ticks)};
			default: assert(0);
		}
	}
	return Mode{Delay()};
}

Mode Delay::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > (info.panel.speed_dial.get()+1)*5 || info.since_switch > 8) return get_auto1(info);
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
