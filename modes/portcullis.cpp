#include "portcullis.h"
#include "teleop.h"
#include "../control/tilt_presets.h"

using namespace std;

unique_ptr<Mode> Auto_portcullis::next_mode(Next_mode_info info){
	
	if(!info.autonomous) return make_unique<Teleop>();
	if(info.since_switch > 4/*2.5*/) return make_unique<Auto_portcullis_done>();
	return make_unique<Auto_portcullis>();
}

Toplevel::Goal Auto_portcullis::run(Run_info info){
	Tilt_presets tilt_presets=read_tilt_presets();
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

bool Auto_portcullis::operator==(Auto_portcullis const&)const{
	return 1;
}

unique_ptr<Mode> Auto_portcullis_done::next_mode(Next_mode_info info){
	if(info.since_switch > 2.5 || !info.autonomous) return make_unique<Teleop>();
	return make_unique<Auto_portcullis_done>();
}

Toplevel::Goal Auto_portcullis_done::run(Run_info){
	Tilt_presets tilt_presets=read_tilt_presets();
	Tilt::Goal top=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));

	Toplevel::Goal goals;
	goals.collector.tilt=top;
	return {};
}

bool Auto_portcullis_done::operator==(Auto_portcullis_done const&)const{
	return 1;
}

#ifdef PORTCULLIS_TEST
int main(){
	{
		Auto_portcullis a;
		test_mode(a);
	}
	{
		Auto_portcullis_done a;
		test_mode(a);
	}
}
#endif 
