#include "climb_release.h"
#include "stdlib.h"

#define INOUT_ADDRESS 4

std::ostream& operator<<(std::ostream& o,Climb_release::Goal a){ 
	#define X(name) if(a==Climb_release::Goal::name) return o<<"Climb_release::Goal("#name")";
	X(IN) X(STOP) X(OUT)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Climb_release::Input){ return o<<"Climb_release::Input()"; }
std::ostream& operator<<(std::ostream& o,Climb_release::Status_detail a){
	#define X(name) if(a==Climb_release::Status_detail::name) return o<<"Climb_release::Status_detail("#name")";
	X(IN) X(OUT) X(UNKNOWN)
	#undef X
	assert(0);
}
std::ostream& operator<<(std::ostream& o,Climb_release::Estimator a){ return o<<"Climb_release::Estimator( last:"<<a.last<<" timer:"<<a.timer<<" last_output:"<<a.last_output<<")"; }
std::ostream& operator<<(std::ostream& o,Climb_release a){ return o<<"Climb_release("<<a.estimator<<")"; }

Climb_release::Estimator::Estimator():last(Climb_release::Status_detail::UNKNOWN),timer(),last_output(Climb_release::Output::STOP){}

bool operator==(Climb_release::Input,Climb_release::Input){ return true; }
bool operator!=(Climb_release::Input,Climb_release::Input){ return false; }
bool operator<(Climb_release::Input,Climb_release::Input){ return false; }

bool operator==(Climb_release::Input_reader,Climb_release::Input_reader){ return true; }
bool operator<(Climb_release::Input_reader,Climb_release::Input_reader){ return false; }

bool operator==(Climb_release::Estimator a,Climb_release::Estimator b){ return a.last==b.last && a.timer==b.timer && a.last_output==b.last_output; }
bool operator!=(Climb_release::Estimator a,Climb_release::Estimator b){ return !(a==b); }

bool operator==(Climb_release::Output_applicator,Climb_release::Output_applicator){ return true; }

bool operator==(Climb_release a,Climb_release b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Climb_release a,Climb_release b){ return !(a==b); }

Climb_release::Input Climb_release::Input_reader::operator()(Robot_inputs)const{ return {}; }
Robot_inputs Climb_release::Input_reader::operator()(Robot_inputs r,Climb_release::Input)const{ return r; }

Climb_release::Output Climb_release::Output_applicator::operator()(Robot_outputs r)const{
	return (r.relay[INOUT_ADDRESS]==Relay_output::_01? Climb_release::Output::OUT : (r.relay[INOUT_ADDRESS]==Relay_output::_10? Climb_release::Output::IN : Climb_release::Output::STOP));
}

Robot_outputs Climb_release::Output_applicator::operator()(Robot_outputs r,Climb_release::Output out)const{
	r.relay[INOUT_ADDRESS]=[&]{
		switch(out){
			case Climb_release::Output::IN: return Relay_output::_10;
			case Climb_release::Output::OUT: return Relay_output::_01;
			case Climb_release::Output::STOP: return Relay_output::_00;
			default: assert(0);
		}
	}();
	return r;
}

Climb_release::Status_detail Climb_release::Estimator::get()const{ return last; }
void Climb_release::Estimator::update(Time time,Climb_release::Input,Climb_release::Output output){
	timer.update(time,true);
	if(output!=last_output && (output==Climb_release::Output::OUT || output==Climb_release::Output::IN)){
		timer.set(1);
	}
	if(timer.done()){
		if(output==Climb_release::Output::IN)last=Climb_release::Status_detail::IN;
		else if(output==Climb_release::Output::OUT)last=Climb_release::Status_detail::OUT;
	}
	last_output=output;
} 

std::set<Climb_release::Input> examples(Climb_release::Input*){ return {{}}; }
std::set<Climb_release::Goal> examples(Climb_release::Goal*){ return {Climb_release::Goal::IN,Climb_release::Goal::STOP,Climb_release::Goal::OUT}; }
std::set<Climb_release::Status_detail> examples(Climb_release::Status_detail*){ return {Climb_release::Status_detail::IN,Climb_release::Status_detail::OUT,Climb_release::Status_detail::UNKNOWN}; }

Climb_release::Output control(Climb_release::Status_detail,Climb_release::Goal goal){
	switch(goal){
		case Climb_release::Goal::IN: return Climb_release::Output::IN;
		case Climb_release::Goal::OUT: return Climb_release::Output::OUT;
		case Climb_release::Goal::STOP: return Climb_release::Output::STOP;
		default: assert(0);
	}
}
Climb_release::Status status(Climb_release::Status_detail a){ return a; }
bool ready(Climb_release::Status,Climb_release::Goal){ return true; }

#ifdef CLIMB_RELEASE_TEST
#include "formal.h"

int main(){
	Climb_release a;
	tester(a);
}

#endif

