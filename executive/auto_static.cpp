#include "auto_static.h"
#include "auto_stop.h"
#include "teleop.h"

using namespace std;

Executive Auto_static::next_mode(Next_mode_info info){
	if(!info.autonomous) return Executive{Teleop()};
	if(info.since_switch > 2) return Executive{Auto_stop()};
	return Executive{Auto_static()};
}

Toplevel::Goal Auto_static::run(Run_info){
	Toplevel::Goal goals;
	goals.drive.left=-1;
	goals.drive.right=-1;
	return goals;
}

bool Auto_static::operator==(Auto_static const&)const{
	return 1;
}

#ifdef AUTO_STATIC_TEST
#include "test.h"
int main(){
	Auto_static a;
	test_executive(a);
}
#endif 
