#include "low_bar_with_low_score.h"

unique_ptr<Mode> Low_bar_with_low_score::next_mode(Next_mode_info){
	return make_unique<Low_bar_with_low_score>();
}

Toplevel::Goal Low_bar_with_low_score::run(Run_info){
	return {};
}

#ifdef LOW_BAR_WITH_LOW_SCORE_TEST
int main(){
	
}
#endif 
