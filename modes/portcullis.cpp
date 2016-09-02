#include "portcullis.h"
#include "teleop.h"
#include "../control/tilt_presets.h"

unique_ptr<Mode> Auto_portcullis::next_mode(Next_mode_info info){
	
	if(!info.autonomous) return make_unique<Teleop>();
	if(info.since_switch > 4/*2.5*/) return make_unique<Auto_portcullis_done>();
	return make_unique<Auto_portcullis>();
}

Toplevel::Goal Auto_portcullis::run(Run_info info){
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

unique_ptr<Mode> Auto_portcullis_done::next_mode(Next_mode_info info){
	if(info.since_switch > 2.5 || !info.autonomous) return make_unique<Teleop>();
	return make_unique<Auto_portcullis_done>();
}

Toplevel::Goal Auto_portcullis_done::run(Run_info){
	Tilt_presets tilt_presets;
	Tilt::Goal top=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));

	Toplevel::Goal goals;
	goals.collector.tilt=top;
	return {};
}



#ifdef PORTCULLIS_TEST
int main(){
	
}
#endif 
