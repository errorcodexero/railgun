#include "cheval.h"

unique_ptr<Mode> Cheval::next_mode(Next_mode_info){
	return make_unique<Cheval>();
}

Toplevel::Goal Cheval::run(Run_info){
	return {};
}

#ifdef CHEVAL_TEST
int main(){
	
}
#endif 
