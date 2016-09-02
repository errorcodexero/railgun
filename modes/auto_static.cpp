#include "auto_static.h"

#include "auto_stop.h"
#include "teleop.h"

unique_ptr<Mode> Auto_static::next_mode(Next_mode_info info){
	if(!info.autonomous) return make_unique<Teleop>();
	if(info.since_switch > 2) return make_unique<Auto_stop>();
	return make_unique<Auto_static>();
}

Toplevel::Goal Auto_static::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=-1;
	goals.drive.right=-1;
	return goals;
}

#ifdef AUTO_STATIC_TEST
int main(){
	
}
#endif 
