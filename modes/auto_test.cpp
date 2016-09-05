#include "auto_test.h"
#include "teleop.h"

using namespace std;

unique_ptr<Mode> Auto_test::next_mode(Next_mode_info info){
	if(info.since_switch > 1 || !info.autonomous) return make_unique<Teleop>();
	return make_unique<Auto_test>();
}

Toplevel::Goal Auto_test::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left = .1;
	goals.drive.right = .1;
	return goals;
}

#ifdef AUTO_TEST_TEST
int main(){
	
}
#endif 
