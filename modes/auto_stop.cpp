#include "auto_stop.h"

unique_ptr<Mode> Auto_stop::next_mode(Next_mode_info){
	return make_unique<Auto_stop>();
}

Toplevel::Goal Auto_stop::run(Run_info){
	return {};
}

#ifdef AUTO_STOP_TEST
int main(){
	
}
#endif 
