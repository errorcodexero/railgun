#include "auto_reach.h"

unique_ptr<Mode> Auto_reach::next_mode(Next_mode_info){
	return make_unique<Auto_reach>();
}

Toplevel::Goal Auto_reach::run(Run_info){
	return {};
}

#ifdef AUTO_REACH_TEST
int main(){
	
}
#endif 
