#ifndef LOW_BAR_LOW_SCORE_H
#define LOW_BAR_LOW_SCORE_H

#include "executive.h"

struct Auto_lbls_cross_lb: public Executive_impl<Auto_lbls_cross_lb>{
	std::pair<int,int> initial_encoders;

	Auto_lbls_cross_lb():initial_encoders(std::make_pair(0,0)){}
	Auto_lbls_cross_lb(std::pair<int,int> ie):initial_encoders(ie){}

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_cross_lb const&)const;
};

struct Auto_lbls_cross_mu: public Executive_impl<Auto_lbls_cross_mu>{
	bool topready;

	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_cross_mu const&)const;
	Auto_lbls_cross_mu(){ topready = false; }
};

struct Auto_lbls_score_seek: public Executive_impl<Auto_lbls_score_seek>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_score_seek const&)const;
};

struct Auto_lbls_score_locate: public Executive_impl<Auto_lbls_score_locate>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_score_locate const&)const;
};

struct Auto_lbls_score_cd: public Executive_impl<Auto_lbls_score_cd>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_score_cd const&)const;
};

struct Auto_lbls_score_eject: public Executive_impl<Auto_lbls_score_eject>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_lbls_score_eject const&)const;
};

#endif
