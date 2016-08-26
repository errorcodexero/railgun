#include "delay.h"

unique_ptr<Mode> Delay::next_mode(Next_mode_info){
	return make_unique<Delay>();
}

Toplevel::Goal Delay::run(Run_info){
	return {};
}

#ifdef DELAY_TEST
int main(){
	
}
#endif 
