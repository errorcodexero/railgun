#include "portcullis.h"

unique_ptr<Mode> Auto_portcullis::next_mode(Next_mode_info){
	return make_unique<Auto_portcullis>();
}

Toplevel::Goal Auto_portcullis::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_portcullis_done::next_mode(Next_mode_info){
	return make_unique<Auto_portcullis_done>();
}

Toplevel::Goal Auto_portcullis_done::run(Run_info){
	return {};
}



#ifdef PORTCULLIS_TEST
int main(){
	
}
#endif 
