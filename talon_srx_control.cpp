#include "talon_srx_control.h"
#include "WPILib.h"
#include <cmath>

using namespace std;

Talon_srx_control::Talon_srx_control():talon(NULL),mode(Talon_srx_control::Mode::INIT){}

Talon_srx_control::Talon_srx_control(int CANBusAddress) {
	talon = new CANTalon(CANBusAddress);
}

Talon_srx_control::~Talon_srx_control(){
	delete talon;
}

ostream& operator<<(ostream& o,Talon_srx_control::Mode a){
	#define X(NAME) if(a==Talon_srx_control::Mode::NAME) return o<<""#NAME;
	TALON_SRX_MODES
	#undef X
	assert(0);
}

void Talon_srx_control::output(ostream& o)const{
	o<<"Talon_srx_control( mode"<<mode<<" out:"<<out<<" in:"<<in<<")";
}

ostream& operator<<(ostream& o,Talon_srx_control a){
	a.output(o);
	return o;
}

bool pid_approx(Talon_srx_output::PID_coefficients a,Talon_srx_output::PID_coefficients b){
	const float TOLERANCE=.001;
	return fabs(a.p-b.p)<TOLERANCE && fabs(a.i-b.i)<TOLERANCE && fabs(a.d-b.d)<TOLERANCE;
}

void Talon_srx_control::set(Talon_srx_output a, bool enable) {
	if(!enable){
		if(mode!=Talon_srx_control::Mode::DISABLE){ 
			talon->Set(0);
			talon->Disable();
			mode=Talon_srx_control::Mode::DISABLE;
		}
		return;
	}
	if(a.mode==Talon_srx_output::Mode::VOLTAGE){
		if(mode!=Talon_srx_control::Mode::VOLTAGE){
			talon->SetControlMode(CANTalon::kVoltage);
			talon->EnableControl();
			talon->SetExpiration(2.0);
			talon->Set(a.power_level);
			out=a;
			mode=Talon_srx_control::Mode::VOLTAGE;
		} else if(a.speed!=out.speed){
			talon->Set(a.speed);
			out.speed=a.speed;
		}	
	} else if(a.mode==Talon_srx_output::Mode::SPEED){
		if(mode!=Talon_srx_control::Mode::SPEED || !pid_approx(out.pid,a.pid)){
			talon->SetControlMode(CANTalon::kSpeed);
			talon->SetPID(a.pid.p,a.pid.i,a.pid.d);	
			talon->EnableControl();
			talon->ConfigEncoderCodesPerRev(1);
			talon->SetExpiration(2.0);
			talon->Set(a.speed);
			out=a;
			mode=Talon_srx_control::Mode::SPEED;
		} else if(a.speed!=out.speed){ 
			talon->Set(a.speed);
			out.speed=a.speed;
		}
	}
}

Talon_srx_input Talon_srx_control::get(){
	if(mode==Talon_srx_control::Mode::VOLTAGE){
		in.current=talon->GetBusVoltage();
	} else if(mode==Talon_srx_control::Mode::SPEED){
		in.velocity=talon->GetSpeed();
	}
	return in;
}
