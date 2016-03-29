#ifndef TALON_SRX_CONTROL_H
#define TALON_SRX_CONTROL_H

#include "util/interface.h"

class CANTalon;

class Talon_srx_control {
	CANTalon *talon;
	
	Talon_srx_output out;
public:
	Talon_srx_control();
	explicit Talon_srx_control(int);
	
	void set(Talon_srx_output, bool);
};

#endif
