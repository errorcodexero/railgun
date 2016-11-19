#include "auto_stop.h"
#include "teleop.h"

using namespace std;

Executive Auto_stop::next_mode(Next_mode_info){
	return Executive{Teleop()};
}

Toplevel::Goal Auto_stop::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=0;
	goals.drive.right=0;
	return goals;
}

bool Auto_stop::operator==(Auto_stop const&)const{
	return 1;
}

#ifdef AUTO_STOP_TEST
#include "test.h"
int main(){
	Auto_stop a;
	test_executive(a);
}
#endif 
