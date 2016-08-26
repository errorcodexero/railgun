#include "auto_statictwo.h"

unique_ptr<Mode> Auto_statictwo::next_mode(Next_mode_info){
	return make_unique<Auto_statictwo>();
}

Toplevel::Goal Auto_statictwo::run(Run_info){
	return {};
}

#ifdef AUTO_STATICTWO_TEST
int main(){
	
}
#endif 
