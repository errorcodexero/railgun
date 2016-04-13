#include "shooter.h"
#include "stdlib.h"

#define SHOOTER_WHEEL_LOC 0
#define BEAM_SENSOR_DIO 5
#define GROUND_RPM 1000
#define CLIMB_RPM 750
#define FREE_SPIN_RPM -750

Shooter::Status_detail::Status_detail():speed(0),beam(0){}
Shooter::Status_detail::Status_detail(int s,bool b):speed(s),beam(b){}
Shooter::Estimator::Estimator():last({}),speed_up_timer({}),last_output(Shooter::Output::Mode::CLIMB_SPEED){}
Shooter::Goal::Goal():mode(Talon_srx_output::Mode::VOLTAGE),type(Shooter::Goal::Type::STOP){}
Shooter::Goal::Goal(Shooter::Goal::Type t):mode(Talon_srx_output::Mode::VOLTAGE),type(t){}
Shooter::Goal::Goal(Talon_srx_output::Mode tm,Shooter::Goal::Type t):mode(tm),type(t){}
Shooter::Output::Output():talon_mode(Talon_srx_output::Mode::VOLTAGE){}
Shooter::Output::Output(Shooter::Output::Mode m):mode(m),talon_mode(Talon_srx_output::Mode::VOLTAGE){}
Shooter::Output::Output(Shooter::Output::Mode m,Talon_srx_output::Mode tm):mode(m),talon_mode(tm){}

std::ostream& operator<<(std::ostream& o,Shooter::Goal::Type a){
	#define X(name) if(a==Shooter::Goal::Type::name) return o<<#name")";
	SHOOTER_GOALS
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Shooter::Goal goal){
	o<<"Shooter::Goal(";
	o<<" mode:"<<goal.mode;
	return o<<" type:"<<goal.type<<")";
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
	if(b.speed<a.speed) return false;
	if(a.enabled<b.enabled) return true;
	if(b.enabled<a.enabled) return false;
	return a.beam && !b.beam;
} 

bool operator<(Shooter::Status_detail a,Shooter::Status_detail b){
	if(a.speed<b.speed) return true;
	if(b.speed<a.speed) return false;
	return a.beam && !b.beam;
}

bool operator==(Shooter::Goal a,Shooter::Goal b){ return a.mode==b.mode && a.type==b.type; }
bool operator!=(Shooter::Goal a,Shooter::Goal b){ return !(a==b); }
bool operator<(Shooter::Goal a,Shooter::Goal b){
	if(a.type<b.type) return true;
	if(b.type<a.type) return false;
	return a.mode<b.mode;
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

void Shooter::Goal::operator()(Talon_srx_output::Mode const& m){
	mode=m;
}

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
	switch(r.talon_srx[SHOOTER_WHEEL_LOC].mode){
		case Talon_srx_output::Mode::VOLTAGE: 
			out.mode=[&]{
				double power = r.talon_srx[SHOOTER_WHEEL_LOC].power_level;
				if (power==0) return Shooter::Output::Mode::STOP;
				if (power==-.5) return Shooter::Output::Mode::GROUND_SPEED;
				if (power==-1) return Shooter::Output::Mode::CLIMB_SPEED;
				if (power==1) return Shooter::Output::Mode::FREE_SPIN;
				assert(0);
			}();
			break;
		case Talon_srx_output::Mode::SPEED: 
			out.mode=[&]{
				double speed = r.talon_srx[SHOOTER_WHEEL_LOC].speed;
				if(speed==0) return Shooter::Output::Mode::STOP;
				if(speed==GROUND_RPM) return Shooter::Output::Mode::GROUND_SPEED;
				if(speed==CLIMB_RPM) return Shooter::Output::Mode::CLIMB_SPEED;
				if(speed==FREE_SPIN_RPM) return Shooter::Output::Mode::FREE_SPIN;
				assert(0);
			}();
			break;
		default: assert(0);
	}
	return out;
}

Robot_outputs Shooter::Output_applicator::operator()(Robot_outputs r,Shooter::Output out)const{ 
	r.talon_srx[SHOOTER_WHEEL_LOC].mode=out.talon_mode;
	switch(out.talon_mode){
		case Talon_srx_output::Mode::VOLTAGE:
			r.talon_srx[SHOOTER_WHEEL_LOC].power_level = [&]{
				switch(out.mode){
					case Shooter::Output::Mode::STOP: return 0.0;
					case Shooter::Output::Mode::GROUND_SPEED: return -.5;
					case Shooter::Output::Mode::CLIMB_SPEED: return -1.0;
					case Shooter::Output::Mode::FREE_SPIN: return 1.0;
					default: assert(0);
				}
			}();
			break;
		case Talon_srx_output::Mode::SPEED:
			r.talon_srx[SHOOTER_WHEEL_LOC].speed = [&]{
				switch(out.mode){
					case Shooter::Output::Mode::STOP: return 0.0;
					case Shooter::Output::Mode::GROUND_SPEED: return (double)GROUND_RPM;
					case Shooter::Output::Mode::CLIMB_SPEED: return (double)CLIMB_RPM;
					case Shooter::Output::Mode::FREE_SPIN: return (double)FREE_SPIN_RPM;
					default: assert(0);
				}
			}();
			break;
		default: assert(0);
	}
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
				last.speed=FREE_SPIN_RPM;
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
	#define X(name) s.insert({Talon_srx_output::Mode::VOLTAGE,Shooter::Goal::Type::name});
	SHOOTER_GOALS
	#undef X
	#define X(name) s.insert({Talon_srx_output::Mode::SPEED,Shooter::Goal::Type::name});
	SHOOTER_GOALS
	#undef X
	return s;
}
std::set<Shooter::Status_detail> examples(Shooter::Status_detail*){
	std::set<Shooter::Status_detail> s;
	bool beam=false;
	for(unsigned int i=0; i<2; i++){
		s.insert({0,beam});
		s.insert({GROUND_RPM,beam});
		s.insert({CLIMB_RPM,beam});
		s.insert({FREE_SPIN_RPM,beam});
		beam=true;
	}
	return s;
}

std::set<Shooter::Output> examples(Shooter::Output*){
	std::set<Shooter::Output> s;
	#define X(name) s.insert({Shooter::Output::Mode::name,Talon_srx_output::Mode::VOLTAGE});
	SHOOTER_MODES
	#undef X
	#define X(name) s.insert({Shooter::Output::Mode::name,Talon_srx_output::Mode::SPEED});
	SHOOTER_MODES
	#undef X
	return s;
}

Shooter::Output control(Shooter::Status_detail, Shooter::Goal goal){
	Shooter::Output out;
	out.talon_mode=goal.mode;
	out.mode=[&]{
		switch(goal.type){
			case Shooter::Goal::Type::STOP: return Shooter::Output::Mode::STOP;
			case Shooter::Goal::Type::GROUND_SHOT: return Shooter::Output::Mode::GROUND_SPEED;
			case Shooter::Goal::Type::CLIMB_SHOT: return Shooter::Output::Mode::CLIMB_SPEED;
			case Shooter::Goal::Type::X: return Shooter::Output::Mode::FREE_SPIN;
			default: assert(0);
		}
	}();
	return out;
}
Shooter::Status status(Shooter::Status_detail a){
	return a;
}

bool ready(Shooter::Status status,Shooter::Goal goal){
	switch(goal.type){
		case Shooter::Goal::Type::STOP: return status.speed==0;
		case Shooter::Goal::Type::GROUND_SHOT: return status.speed==GROUND_RPM;
		case Shooter::Goal::Type::CLIMB_SHOT: return status.speed==CLIMB_RPM;
		case Shooter::Goal::Type::X: return true;
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

