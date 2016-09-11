#ifndef PORTCULLIS_H
#define PORTCULLIS_H

#include "mode.h"

struct Auto_portcullis: public Mode_impl<Auto_portcullis>{
	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_portcullis const&)const;
};

struct Auto_portcullis_done: public Mode_impl<Auto_portcullis_done>{
	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_portcullis_done const&)const;
};
#endif
