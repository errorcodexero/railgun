#include "auto_null.h"
#include "teleop.h"
unique_ptr<Mode> Auto_null::next_mode(Next_mode_info){
	return make_unique<Teleop>();
}

Toplevel::Goal Auto_null::run(Run_info){
	return {};
}

#ifdef AUTO_NULL_TEST
int main(){
	
}
#endif 
