#include "cheval.h"

using namespace std;

unique_ptr<Mode> Auto_cheval_pos::next_mode(Next_mode_info){
	return make_unique<Auto_cheval_pos>();
}

Toplevel::Goal Auto_cheval_pos::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_cheval_wait::next_mode(Next_mode_info){
	return make_unique<Auto_cheval_wait>();
}

Toplevel::Goal Auto_cheval_wait::run(Run_info){
	return {};
}

#ifdef CHEVAL_TEST
int main(){
	
}
#endif 
