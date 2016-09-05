#include "auto_null.h"
#include "teleop.h"

using namespace std;

unique_ptr<Mode> Auto_null::next_mode(Next_mode_info){
	return make_unique<Teleop>();
}

Toplevel::Goal Auto_null::run(Run_info){
	return {};
}

bool Auto_null::operator==(Auto_null const&)const{
	return 1;
}

#ifdef AUTO_NULL_TEST
int main(){
	Auto_null a;
	test_mode(a);
}
#endif 
