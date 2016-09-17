#include "auto_null.h"
#include "teleop.h"

using namespace std;

Executive Auto_null::next_mode(Next_mode_info){
	return Executive{Teleop()};
}

Toplevel::Goal Auto_null::run(Run_info){
	return {};
}

bool Auto_null::operator==(Auto_null const&)const{
	return 1;
}

#ifdef AUTO_NULL_TEST
#include "test.h"
int main(){
	Auto_null a;
	test_executive(a);
}
#endif 
