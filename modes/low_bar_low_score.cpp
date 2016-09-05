#include "low_bar_low_score.h"

using namespace std;

unique_ptr<Mode> Auto_lbls_cross_lb::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_cross_lb>();
}

Toplevel::Goal Auto_lbls_cross_lb::run(Run_info){
	return {};
}

unique_ptr<Mode> Auto_lbls_cross_mu::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_cross_mu>();
}

Toplevel::Goal Auto_lbls_cross_mu::run(Run_info){
	return {};
}


unique_ptr<Mode> Auto_lbls_score_seek::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_score_seek>();
}

Toplevel::Goal Auto_lbls_score_seek::run(Run_info){
	return {};
}


unique_ptr<Mode> Auto_lbls_score_locate::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_score_locate>();
}

Toplevel::Goal Auto_lbls_score_locate::run(Run_info){
	return {};
}


unique_ptr<Mode> Auto_lbls_score_cd::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_score_cd>();
}

Toplevel::Goal Auto_lbls_score_cd::run(Run_info){
	return {};
}


unique_ptr<Mode> Auto_lbls_score_eject::next_mode(Next_mode_info){
	return make_unique<Auto_lbls_score_eject>();
}

Toplevel::Goal Auto_lbls_score_eject::run(Run_info){
	return {};
}
	#define STEPS\
		X(cross_lb)\
		X(cross_mu)\
		X(score_seek)\
		X(score_locate)\
		X(score_cd)\
		X(score_eject)

#define X(NAME) bool Auto_lbls_##NAME::operator==(Auto_lbls_##NAME const&)const{ return 1; }
STEPS
#undef X

#ifdef LOW_BAR_LOW_SCORE_TEST
int main(){

	#define X(NAME) { Auto_lbls_##NAME a; test_mode(a); }
	STEPS
	#undef X
}
#endif 
