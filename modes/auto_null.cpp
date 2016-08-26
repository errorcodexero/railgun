#include "auto_null.h"

unique_ptr<Mode> Auto_null::next_mode(Next_mode_info){
	return make_unique<Auto_null>();
}

Toplevel::Goal Auto_null::run(Run_info){
	return {};
}

#ifdef AUTO_NULL_TEST
int main(){
	
}
#endif 
