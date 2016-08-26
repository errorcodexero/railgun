#include "barrelracer.h"

unique_ptr<Mode> Barrelracer::next_mode(Next_mode_info){
	return make_unique<Barrelracer>();
}

Toplevel::Goal Barrelracer::run(Run_info){
	return {};
}

#ifdef BARRELRACER_TEST
int main(){
	
}
#endif 
