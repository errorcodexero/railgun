#ifndef LOW_BAR_LOW_SCORE_H
#define LOW_BAR_LOW_SCORE_H

#include "mode.h"

struct Low_bar_low_score: public Mode{
	unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
};

#endif
