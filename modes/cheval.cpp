#include "cheval.h"

using namespace std;

unique_ptr<Mode> Auto_cheval_pos::next_mode(Next_mode_info){
	return make_unique<Auto_cheval_pos>();
}

Toplevel::Goal Auto_cheval_pos::run(Run_info){
	return {};
}

bool Auto_cheval_pos::operator==(Auto_cheval_pos const&)const{
	return 1;
}

unique_ptr<Mode> Auto_cheval_wait::next_mode(Next_mode_info){
	return make_unique<Auto_cheval_wait>();
}

Toplevel::Goal Auto_cheval_wait::run(Run_info){
	return {};
}

bool Auto_cheval_wait::operator==(Auto_cheval_wait const&)const{
	return 1;
}

#ifdef CHEVAL_TEST
int main(){
	{
		Auto_cheval_pos a;
		test_mode(a);
	}
	{
		Auto_cheval_wait a;
		test_mode(a);
	}
}
#endif 
