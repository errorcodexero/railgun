#include "auto_statictwo.h"
#include "teleop.h" 
#include "auto_stop.h"

using namespace std;

Executive Auto_statictwo::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	if(info.since_switch > 2.5) return Executive{Auto_stop()};
	return Executive{Auto_statictwo()};
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
#include "test.h"
int main(){
	Auto_statictwo a;
	test_executive(a);
}
#endif 
