#include "auto_static.h"
#include "auto_stop.h"
#include "teleop.h"

using namespace std;

Mode Auto_static::next_mode(Next_mode_info info){
	if(!info.autonomous) return Mode{Teleop()};
	if(info.since_switch > 2) return Mode{Auto_stop()};
	return Mode{Auto_static()};
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
int main(){
	Auto_static a;
	test_mode(a);
}
#endif 
