#include "portcullis.h"

unique_ptr<Mode> Portcullis::next_mode(Next_mode_info){
	return make_unique<Portcullis>();
}

Toplevel::Goal Portcullis::run(Run_info){
	return {};
}

#ifdef PORTCULLIS_TEST
int main(){
	
}
#endif 
