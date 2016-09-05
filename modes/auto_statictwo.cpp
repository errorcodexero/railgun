#include "auto_statictwo.h"
#include "teleop.h" 
#include "auto_stop.h"

using namespace std;

unique_ptr<Mode> Auto_statictwo::next_mode(Next_mode_info info){
	if(!info.autonomous) return make_unique<Teleop>();
	if(info.since_switch > 2.5) return make_unique<Auto_stop>();
	return make_unique<Auto_statictwo>();
}

Toplevel::Goal Auto_statictwo::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=-.5;
	goals.drive.right=-.5;
	return goals;
}

#ifdef AUTO_STATICTWO_TEST
int main(){
	
}
#endif 
