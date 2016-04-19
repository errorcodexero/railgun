#include "talon_srx_control.h"
#include "WPILib.h"
#include "../util/util.h"
#include <cmath>
#include <cassert>

using namespace std;

Talon_srx_control::Talon_srx_control():talon(NULL),since_query(0),mode(Mode::INIT){}

Talon_srx_control::Talon_srx_control(int CANBusAddress):talon(NULL),since_query(0),mode(Mode::INIT) {
	init(CANBusAddress);
}

Talon_srx_control::~Talon_srx_control(){
	delete talon;
}

void Talon_srx_control::init(int CANBusAddress){
	assert(!talon);
	assert(mode==Mode::INIT);
	talon = new CANTalon(CANBusAddress);
	assert(talon);
	talon->SetSafetyEnabled(false);
	mode = Mode::VOLTAGE;
}

ostream& operator<<(ostream& o,Talon_srx_control::Mode a){
	#define X(NAME) if(a==Talon_srx_control::Mode::NAME) return o<<""#NAME;
	TALON_SRX_MODES
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Talon_srx_control a){
	o<<"Talon_srx_control( mode:"<<a.mode;
	if(!!a.talon) o<<" out:"<<a.out;/*" last_out:"<<a.last_out<<*/
	o<<" init:"<<!!a.talon;
	o<<" since_query:"<<a.since_query;
	o<<" in:"<<a.in;
	return o<<")";
}

bool pid_approx(PID_values a,PID_values b){
	const float TOLERANCE=.001;
	return fabs(a.p-b.p)<TOLERANCE && fabs(a.i-b.i)<TOLERANCE && fabs(a.d-b.d)<TOLERANCE && fabs(a.f-b.f)<TOLERANCE;
}

void Talon_srx_control::set(Talon_srx_output a, bool enable) {
	static const float EXPIRATION=2.0;
	assert(mode!=Mode::INIT);
	if(!enable){
		if(mode!=Talon_srx_control::Mode::DISABLE){ 
			talon->Set(0);
			talon->SetSafetyEnabled(false);
			talon->Disable();
			mode=Talon_srx_control::Mode::DISABLE;
		}
		return;
	}
	if(a.mode==Talon_srx_output::Mode::VOLTAGE){
		assert(a.power_level==clip(a.power_level));
		if(mode!=Talon_srx_control::Mode::VOLTAGE){
			talon->SetControlMode(CANSpeedController::kPercentVbus);
			talon->EnableControl();
			talon->SetExpiration(EXPIRATION);
			talon->SetSafetyEnabled(true);
			talon->Set(a.power_level);
			out=a;
			mode=Talon_srx_control::Mode::VOLTAGE;
		} else if((a.power_level!=out.power_level || since_query==1) /*&& out!=last_out*/){
			talon->Set(a.power_level);
			out.power_level=a.power_level;
		}	
	} else if(a.mode==Talon_srx_output::Mode::SPEED){
		if(mode!=Talon_srx_control::Mode::SPEED || !pid_approx(out.pid,a.pid)){
			talon->SetControlMode(CANSpeedController::kSpeed);
			talon->SetPID(a.pid.p,a.pid.i,a.pid.d,a.pid.f);	
			talon->EnableControl();
			talon->SetFeedbackDevice(CANTalon::QuadEncoder);
			talon->ConfigEncoderCodesPerRev(200);
			talon->SetExpiration(EXPIRATION);
			talon->SetSafetyEnabled(true);
			talon->Set(a.speed);
			out=a;
			mode=Talon_srx_control::Mode::SPEED;
		} else if((a.speed!=out.speed || since_query==1) /*&& out!=last_out*/){ 
			talon->Set(a.speed);
			out.speed=a.speed;
		}
	}
	//last_out=out;
}

Talon_srx_input Talon_srx_control::get(){
	if(since_query>0){
		in.current=talon->GetBusVoltage();
		in.velocity=talon->GetSpeed();
		in.a=talon->GetPinStateQuadA();
		in.b=talon->GetPinStateQuadB();
		in.fwd_limit_switch=talon->IsFwdLimitSwitchClosed();
		in.rev_limit_switch=talon->IsRevLimitSwitchClosed();
		in.encoder_position=talon->GetEncPosition();
		since_query=0;
	}
	since_query++;
	return in;
}

Talon_srx_controls::Talon_srx_controls():init_(false){}

void Talon_srx_controls::init(){
	if(!init_){
		for(unsigned int i=0; i<Robot_outputs::TALON_SRX_OUTPUTS; i++){
			talons[i].init(i);
		}
		init_=true;
	}
}

void Talon_srx_controls::set(Checked_array<Talon_srx_output,Robot_outputs::TALON_SRX_OUTPUTS> const& a,Checked_array<bool,Robot_outputs::TALON_SRX_OUTPUTS> const& enable){
	init();
	for(unsigned int i=0; i<Robot_outputs::TALON_SRX_OUTPUTS; i++){
		talons[i].set(a[i],enable[i]);
	}
}

array<Talon_srx_input,Robot_inputs::TALON_SRX_INPUTS> Talon_srx_controls::get(){
	init();
	
	array<Talon_srx_input,Robot_inputs::TALON_SRX_INPUTS> inputs;
	for(unsigned int i=0; i<Robot_inputs::TALON_SRX_INPUTS; i++){
		inputs[i]=talons[i].get();
	}
	return inputs;
}

ostream& operator<<(ostream& o,Talon_srx_controls const& t){
	o<<"Talon_srx_controls(";
	o<< "init:"<<t.init_;
	for(unsigned int i=0;i<t.talons.size(); i++){
		//o<<" talon_srx_"<<i<<t.talons[i];
	}
	return o<<")";
}

