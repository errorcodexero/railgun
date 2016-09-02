#include "auto_stop.h"

#include "teleop.h"

unique_ptr<Mode> Auto_stop::next_mode(Next_mode_info){
	return make_unique<Teleop>();
}

Toplevel::Goal Auto_stop::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=0;
	goals.drive.right=0;
	return goals;
}

#ifdef AUTO_STOP_TEST
int main(){
	
}
#endif 
