#ifndef TALON_SRX_CONTROL_H
#define TALON_SRX_CONTROL_H

#include "util/interface.h"

class CANTalon;

class Talon_srx_control {
	private:
	CANTalon *talon;
	
	Talon_srx_output out;
	Talon_srx_input in;
	
	public:
	#define TALON_SRX_MODES X(INIT) X(VOLTAGE) X(SPEED) X(DISABLE)
	enum class Mode{
		#define X(NAME) NAME,
		TALON_SRX_MODES
		#undef X
	};

	private:
	Mode mode;
	
	public:
	Talon_srx_control();
	~Talon_srx_control();
	explicit Talon_srx_control(int CANBusAddress);
	
	void set(Talon_srx_output, bool);
	Talon_srx_input get();
	void output(std::ostream&)const;
};

std::ostream& operator<<(std::ostream&,Talon_srx_control::Mode);
std::ostream& operator<<(std::ostream&,Talon_srx_control);
#endif
