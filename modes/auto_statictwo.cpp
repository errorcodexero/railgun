#include "auto_statictwo.h"
#include "teleop.h" 
#include "auto_stop.h"

using namespace std;

Mode Auto_statictwo::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > 2.5) return Mode{Auto_stop()};
	return Mode{Auto_statictwo()};
}

Toplevel::Goal Auto_statictwo::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=-.5;
	goals.drive.right=-.5;
	return goals;
}

bool Auto_statictwo::operator==(Auto_statictwo const&)const{
	return 1;
}

#ifdef AUTO_STATICTWO_TEST
int main(){
	Auto_statictwo a;
	test_mode(a);
}
#endif 
