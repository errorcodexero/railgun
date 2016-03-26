#include "shooter.h"
#include "stdlib.h"

#define SHOOTER_WHEEL_LOC 0
#define GROUND_RPM 1000
#define CLIMB_RPM 750

Shooter::Status_detail::Status_detail():speed(0){}
Shooter::Status_detail::Status_detail(int s):speed(s){}

std::ostream& operator<<(std::ostream& o,Shooter::Goal goal){
	#define X(name) if(goal==Shooter::Goal::name) return o<<"Shooter::Goal("#name")";
	SHOOTER_GOALS
	#undef X
	assert(0);
}
std::ostream& operator<<(std::ostream& o,Shooter::Input a){ return o<<"Shooter::Input( speed:"<<a.speed<<" beam:"<<a.beam<<")"; }
std::ostream& operator<<(std::ostream& o,Shooter::Status_detail a){ return o<<"Shooter::Status_detail( speed:"<<a.speed<<")"; }
std::ostream& operator<<(std::ostream& o,Shooter){ return o<<"Shooter()"; }

std::ostream& operator<<(std::ostream& o,Shooter::Output out){
	#define X(NAME) if(out==Shooter::Output::NAME) return o<<"Shooter::Output("#NAME")";
	SHOOTER_OUTPUTS
	#undef X
	assert(0);
}

bool operator==(Shooter::Input a,Shooter::Input b){ return a.speed==b.speed && a.beam==b.beam; }
bool operator!=(Shooter::Input a,Shooter::Input b){ return !(a==b); }
bool operator<(Shooter::Input a,Shooter::Input b){
	if(a.speed<b.speed) return true;
	if(b.speed<a.speed) return true;
	return a.beam && !b.beam;
} 

bool operator<(Shooter::Status_detail a,Shooter::Status_detail b){
	return a.speed<b.speed;
}
bool operator==(Shooter::Status_detail a,Shooter::Status_detail b){ return (a.speed==b.speed); }
bool operator!=(Shooter::Status_detail a,Shooter::Status_detail b){ return !(a==b); }

bool operator<(Shooter::Input_reader,Shooter::Input_reader){ return false; }
bool operator==(Shooter::Input_reader,Shooter::Input_reader){ return true; }

bool operator==(Shooter::Estimator,Shooter::Estimator){ return true; }
bool operator!=(Shooter::Estimator,Shooter::Estimator){ return false; }

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator){ return true; }

bool operator==(Shooter a,Shooter b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Shooter a,Shooter b){ return !(a==b); }

static const unsigned BEAM_SENSOR_DIO=5;

Shooter::Input Shooter::Input_reader::operator()(Robot_inputs r)const{
	return {r.talon_srx[SHOOTER_WHEEL_LOC].velocity,(r.digital_io.in[BEAM_SENSOR_DIO]==Digital_in::_1)};
}
Robot_inputs Shooter::Input_reader::operator()(Robot_inputs r,Shooter::Input in)const{
	r.talon_srx[SHOOTER_WHEEL_LOC].velocity = in.speed;
	r.digital_io.in[BEAM_SENSOR_DIO]=(in.beam? Digital_in::_1 : Digital_in::_0);
	return r;
}

Shooter::Output Shooter::Output_applicator::operator()(Robot_outputs r)const{
	double power = r.talon_srx[SHOOTER_WHEEL_LOC].power_level;
	if (power==0) return Shooter::Output::STOP;
	if (power==.5) return Shooter::Output::GROUND_SPEED;
	if (power==1) return Shooter::Output::CLIMB_SPEED;
	if (power==-1) return Shooter::Output::FREE_SPIN;
	assert(0);
}
Robot_outputs Shooter::Output_applicator::operator()(Robot_outputs r,Shooter::Output out)const{ 
	r.talon_srx[SHOOTER_WHEEL_LOC].power_level = [&]{
		if (out==Shooter::Output::STOP) return 0.0;
		else if (out==Shooter::Output::GROUND_SPEED) return .5;
		else if (out==Shooter::Output::CLIMB_SPEED) return 1.0;
		else if (out==Shooter::Output::FREE_SPIN) return -1.0;
		assert(0);
	}();
	return r;
}

Shooter::Status_detail Shooter::Estimator::get()const{ return {}; }
void Shooter::Estimator::update(Time,Shooter::Input,Shooter::Output){} 

std::set<Shooter::Input> examples(Shooter::Input*){
	return {{true,true},{true,false},{false,true},{false,false}}; 
}
std::set<Shooter::Goal> examples(Shooter::Goal*){
	std::set<Shooter::Goal> s;
	#define X(name) s.insert(Shooter::Goal::name);
	SHOOTER_GOALS
	#undef X
	return s;
}
std::set<Shooter::Status_detail> examples(Shooter::Status_detail*){
	return {
		Shooter::Status_detail{0}
	};
}
std::set<Shooter::Output> examples(Shooter::Output*){
	std::set<Shooter::Output> s;
	#define X(name) s.insert(Shooter::Output::name);
	SHOOTER_OUTPUTS
	#undef X
	return s;
}

Shooter::Output control(Shooter::Status_detail, Shooter::Goal goal){
	switch(goal){
		case Shooter::Goal::STOP: return Shooter::Output::STOP;
		case Shooter::Goal::GROUND_SHOT: return Shooter::Output::GROUND_SPEED;
		case Shooter::Goal::CLIMB_SHOT: return Shooter::Output::CLIMB_SPEED;
		case Shooter::Goal::X: return Shooter::Output::FREE_SPIN;
		default: assert(0);
	}
}
Shooter::Status status(Shooter::Status_detail a){
	return a;
}

bool ready(Shooter::Status status,Shooter::Goal goal){
	switch(goal){
		case Shooter::Goal::STOP: return status.speed==0;
		case Shooter::Goal::GROUND_SHOT: return status.speed==GROUND_RPM;
		case Shooter::Goal::CLIMB_SHOT: return status.speed==CLIMB_RPM;
		case Shooter::Goal::X: return true;
		default: assert(0);
	}
}

#ifdef SHOOTER_TEST
#include "formal.h"

int main(){
	Shooter a;
	tester(a);
}

#endif

