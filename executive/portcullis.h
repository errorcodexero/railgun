#ifndef PORTCULLIS_H
#define PORTCULLIS_H

#include "executive.h"

struct Auto_portcullis: public Executive_impl<Auto_portcullis>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_portcullis const&)const;
};

struct Auto_portcullis_done: public Executive_impl<Auto_portcullis_done>{
	Executive next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	bool operator==(Auto_portcullis_done const&)const;
};
#endif
