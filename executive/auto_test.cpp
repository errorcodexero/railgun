#include "auto_test.h"
#include "teleop.h"

using namespace std;

Executive Auto_test::next_mode(Next_mode_info info){
	if(info.since_switch > 1 || !info.autonomous) return Executive{Teleop()};
	return Executive{Auto_test()};
}

Toplevel::Goal Auto_test::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left = .1;
	goals.drive.right = .1;
	return goals;
}

bool Auto_test::operator==(Auto_test const&)const{
	return 1;
}

#ifdef AUTO_TEST_TEST
#include "test.h"
int main(){
	Auto_test a;
	test_executive(a);
}
#endif 
