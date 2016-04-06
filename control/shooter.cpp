#include "shooter.h"
#include "stdlib.h"

#define SHOOTER_WHEEL_LOC 0
#define BEAM_SENSOR_DIO 5
#define GROUND_RPM 1000
#define CLIMB_RPM 750

Shooter::Status_detail::Status_detail():speed(0),beam(0){}
Shooter::Status_detail::Status_detail(int s,bool b):speed(s),beam(b){}
Shooter::Estimator::Estimator():last({}),speed_up_timer({}),last_output(Shooter::Output::Mode::CLIMB_SPEED){}
Shooter::Output::Output():talon_mode(Talon_srx_output::Mode::VOLTAGE){}
Shooter::Output::Output(Shooter::Output::Mode m):mode(m),talon_mode(Talon_srx_output::Mode::VOLTAGE){}
Shooter::Output::Output(Shooter::Output::Mode m,Talon_srx_output::Mode tm):mode(m),talon_mode(tm){}

std::ostream& operator<<(std::ostream& o,Shooter::Goal goal){
	#define X(name) if(goal==Shooter::Goal::name) return o<<"Shooter::Goal("#name")";
	SHOOTER_GOALS
	#undef X
	assert(0);
}
std::ostream& operator<<(std::ostream& o,Shooter::Estimator a){ return o<<"Shooter::Estimator( last:"<<a.get()<<" last_output:"<<a.last_output<<" speed_up_timer:"<<a.speed_up_timer<<")"; }
std::ostream& operator<<(std::ostream& o,Shooter::Input a){ return o<<"Shooter::Input( speed:"<<a.speed<<" beam:"<<a.beam<<" enabled:"<<a.enabled<<")"; }
std::ostream& operator<<(std::ostream& o,Shooter::Status_detail a){ return o<<"Shooter::Status_detail( speed:"<<a.speed<<" beam:"<<a.beam<<")"; }
std::ostream& operator<<(std::ostream& o,Shooter a){ return o<<"Shooter("<<a.estimator<<")"; }

std::ostream& operator<<(std::ostream& o,Shooter::Output::Mode mode){
	#define X(NAME) if(mode==Shooter::Output::Mode::NAME) return o<<#NAME;
	SHOOTER_MODES
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Shooter::Output out){
	return o<<"Shooter::Output( mode:"<<out.mode<<" talon_mode:"<<out.talon_mode<<")";
}

bool operator==(Shooter::Input a,Shooter::Input b){ return a.speed==b.speed && a.beam==b.beam; }
bool operator!=(Shooter::Input a,Shooter::Input b){ return !(a==b); }
bool operator<(Shooter::Input a,Shooter::Input b){
	if(a.speed<b.speed) return true;
	if(b.speed<a.speed) return true;
	if(a.enabled<b.enabled) return true;
	return a.beam && !b.beam;
} 

bool operator<(Shooter::Status_detail a,Shooter::Status_detail b){
	if(a.speed<b.speed) return true;
	if(b.speed<a.speed) return true;
	return a.beam && !b.beam;
}
bool operator==(Shooter::Output a,Shooter::Output b){ return a.mode==b.mode && a.talon_mode==b.talon_mode; }
bool operator!=(Shooter::Output a,Shooter::Output b){ return !(a==b); }
bool operator<(Shooter::Output a,Shooter::Output b){
	if(a.mode<b.mode) return true;
	if(b.mode<a.mode) return false;
	return a.talon_mode<b.talon_mode;
}

bool operator==(Shooter::Status_detail a,Shooter::Status_detail b){ return (a.speed==b.speed && a.beam==b.beam); }
bool operator!=(Shooter::Status_detail a,Shooter::Status_detail b){ return !(a==b); }

bool operator<(Shooter::Input_reader,Shooter::Input_reader){ return false; }
bool operator==(Shooter::Input_reader,Shooter::Input_reader){ return true; }

bool operator==(Shooter::Estimator a,Shooter::Estimator b){ return a.last==b.last && a.last_output==b.last_output && a.speed_up_timer==b.speed_up_timer; }
bool operator!=(Shooter::Estimator a,Shooter::Estimator b){ return !(a==b); }

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator){ return true; }

bool operator==(Shooter a,Shooter b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Shooter a,Shooter b){ return !(a==b); }

Shooter::Input Shooter::Input_reader::operator()(Robot_inputs r)const{
	return {r.talon_srx[SHOOTER_WHEEL_LOC].velocity,(r.digital_io.in[BEAM_SENSOR_DIO]==Digital_in::_1),r.robot_mode.enabled};
}
Robot_inputs Shooter::Input_reader::operator()(Robot_inputs r,Shooter::Input in)const{
	r.talon_srx[SHOOTER_WHEEL_LOC].velocity = in.speed;
	r.digital_io.in[BEAM_SENSOR_DIO]=(in.beam? Digital_in::_1 : Digital_in::_0);
	r.robot_mode.enabled=in.enabled;
	return r;
}

Shooter::Output Shooter::Output_applicator::operator()(Robot_outputs r)const{
	Shooter::Output out;
	out.talon_mode=r.talon_srx[SHOOTER_WHEEL_LOC].mode;
	double power = r.talon_srx[SHOOTER_WHEEL_LOC].power_level;
	if (power==0) out.mode=Shooter::Output::Mode::STOP;
	else if (power==-.5) out.mode=Shooter::Output::Mode::GROUND_SPEED;
	else if (power==-1) out.mode=Shooter::Output::Mode::CLIMB_SPEED;
	else if (power==1) out.mode=Shooter::Output::Mode::FREE_SPIN;
	else assert(0);
	return out;
}
Robot_outputs Shooter::Output_applicator::operator()(Robot_outputs r,Shooter::Output out)const{ 
	r.talon_srx[SHOOTER_WHEEL_LOC].mode=out.talon_mode;
	r.talon_srx[SHOOTER_WHEEL_LOC].power_level = [&]{
		switch(out.mode){
			case Shooter::Output::Mode::STOP: return 0.0;
			case Shooter::Output::Mode::GROUND_SPEED: return -.5;
		 	case Shooter::Output::Mode::CLIMB_SPEED: return -1.0;
			case Shooter::Output::Mode::FREE_SPIN: return 1.0;
			default: assert(0);
		}
	}();
	return r;
}

Shooter::Status_detail Shooter::Estimator::get()const{
	return last;
}

void Shooter::Estimator::update(Time time,Shooter::Input in,Shooter::Output output){
	static const float SPEED_UP_TIME=5;
	if(output!=last_output){
		speed_up_timer.set(SPEED_UP_TIME);
		
	}
	speed_up_timer.update(time,in.enabled);
	if(speed_up_timer.done()){
		switch(output.mode){
			case Shooter::Output::Mode::STOP: 
				last.speed=0;
				break;
			case Shooter::Output::Mode::GROUND_SPEED:
				last.speed=GROUND_RPM;
				break;
			case Shooter::Output::Mode::CLIMB_SPEED:
				last.speed=CLIMB_RPM;
				break;
			case Shooter::Output::Mode::FREE_SPIN:
				break; 
			default:
				assert(0);
		}
	}
	last.beam=in.beam;
	//last.speed=in.speed;
	last_output=output;
} 

std::set<Shooter::Input> examples(Shooter::Input*){
	return {
		{true,true,true},
		{true,true,false},
		{true,false,true},
		{true,false,false},
		{false,true,true},
		{false,true,false},
		{false,false,true},
		{false,false,false}
	}; 
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
		{0,0},
		{0,1}
	};
}
std::set<Shooter::Output> examples(Shooter::Output*){
	std::set<Shooter::Output> s;
	#define X(name) s.insert({Shooter::Output::Mode::name,Talon_srx_output::Mode::VOLTAGE});
	SHOOTER_MODES
	#undef X
	return s;
}

Shooter::Output control(Shooter::Status_detail, Shooter::Goal goal){
	switch(goal){
		case Shooter::Goal::STOP: return Shooter::Output::Mode::STOP;
		case Shooter::Goal::GROUND_SHOT: return Shooter::Output::Mode::GROUND_SPEED;
		case Shooter::Goal::CLIMB_SHOT: return Shooter::Output::Mode::CLIMB_SPEED;
		case Shooter::Goal::X: return Shooter::Output::Mode::FREE_SPIN;
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

