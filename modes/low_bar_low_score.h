#ifndef LOW_BAR_LOW_SCORE_H
#define LOW_BAR_LOW_SCORE_H

#include "mode.h"

struct Auto_lbls_cross_lb: public Mode_impl<Auto_lbls_cross_lb>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_cross_lb const&)const;
};

struct Auto_lbls_cross_mu: public Mode_impl<Auto_lbls_cross_mu>{
	bool topready;
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_cross_mu const&)const;
	Auto_lbls_cross_mu(){ topready = false; }
};

struct Auto_lbls_score_seek: public Mode_impl<Auto_lbls_score_seek>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_score_seek const&)const;
};

struct Auto_lbls_score_locate: public Mode_impl<Auto_lbls_score_locate>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_score_locate const&)const;
};

struct Auto_lbls_score_cd: public Mode_impl<Auto_lbls_score_cd>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_score_cd const&)const;
};

struct Auto_lbls_score_eject: public Mode_impl<Auto_lbls_score_eject>{
	std::unique_ptr<Mode> next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_score_eject const&)const;
};

#endif
