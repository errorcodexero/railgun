#include "auto_test.h"

unique_ptr<Mode> Auto_test::next_mode(Next_mode_info){
	return make_unique<Auto_test>();
}

Toplevel::Goal Auto_test::run(Run_info){
	return {};
}

#ifdef AUTO_TEST_TEST
int main(){
	
}
#endif 
