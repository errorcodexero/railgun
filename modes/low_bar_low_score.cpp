#include "low_bar_low_score.h"

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
#ifdef LOW_BAR_LOW_SCORE_TEST
int main(){
	
}
#endif 
