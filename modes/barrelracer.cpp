#include "barrelracer.h"

unique_ptr<Mode> Auto_br_straightaway::next_mode(Next_mode_info){
	return make_unique<Auto_br_straightaway>();
}

Toplevel::Goal Auto_br_straightaway::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_br_initialturn::next_mode(Next_mode_info){
	return make_unique<Auto_br_initialturn>();
}

Toplevel::Goal Auto_br_initialturn::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_br_side::next_mode(Next_mode_info){
	return make_unique<Auto_br_side>();
}

Toplevel::Goal Auto_br_side::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_br_sideturn::next_mode(Next_mode_info){
	return make_unique<Auto_br_sideturn>();
}

Toplevel::Goal Auto_br_sideturn::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_br_endturn::next_mode(Next_mode_info){
	return make_unique<Auto_br_endturn>();
}

Toplevel::Goal Auto_br_endturn::run(Run_info){
	return {};
}

#ifdef BARRELRACER_TEST
int main(){
	
}
#endif 
