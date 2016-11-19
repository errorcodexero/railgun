#include "portcullis.h"
#include "teleop.h"
#include "../control/tilt_presets.h"

using namespace std;

Executive Auto_portcullis::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	if(info.since_switch > 4/*2.5*/) return Executive{Auto_portcullis_done()};
	return Executive{Auto_portcullis()};
}

Toplevel::Goal Auto_portcullis::run(Run_info info){
	Tilt_presets tilt_presets=read_tilt_presets();
	Toplevel::Goal goals;
	Tilt::Goal low=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.low));

	goals.collector.front=Front::Goal::OFF;
	goals.collector.sides=Sides::Goal::OFF;
	goals.collector.tilt=low;
	if(ready(info.toplevel_status.collector.tilt.angle,goals.collector.tilt)){	
		goals.drive.left=-.5;
		goals.drive.right=-.5;	
	}		
	return goals;
}

bool Auto_portcullis::operator==(Auto_portcullis const&)const{
	return 1;
}

Executive Auto_portcullis_done::next_mode(Next_mode_info info){
	if(info.since_switch > 2.5 || !info.autonomous) return Executive{Teleop()};
	return Executive{Auto_portcullis_done()};
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
#include "test.h"
int main(){
	{
		Auto_portcullis a;
		test_executive(a);
	}
	{
		Auto_portcullis_done a;
		test_executive(a);
	}
}
#endif 
