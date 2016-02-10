#include "inout.h"
#include "stdlib.h"

#define INOUT_ADDRESS 4

std::ostream& operator<<(std::ostream& o,Inout::Goal a){ 
	#define X(name) if(a==Inout::Goal::name) return o<<"Inout::Goal("#name")";
	X(IN) X(STOP) X(OUT)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Inout::Input){ return o<<"Inout::Input{}"; }
std::ostream& operator<<(std::ostream& o,Inout::Status_detail){ return o<<"Inout::Status_detail"; }
std::ostream& operator<<(std::ostream& o,Inout){ return o<<"Inout{}"; }

bool operator==(Inout::Input,Inout::Input){ return true; }
bool operator!=(Inout::Input,Inout::Input){ return false; }
bool operator<(Inout::Input,Inout::Input){ return false; }

bool operator<(Inout::Status_detail,Inout::Status_detail){ return false; }
bool operator==(Inout::Status_detail,Inout::Status_detail){ return true; }
bool operator!=(Inout::Status_detail,Inout::Status_detail){ return false; }

bool operator==(Inout::Input_reader,Inout::Input_reader){ return true; }
bool operator<(Inout::Input_reader,Inout::Input_reader){ return false; }

bool operator==(Inout::Estimator,Inout::Estimator){ return true; }
bool operator!=(Inout::Estimator,Inout::Estimator){ return false; }

bool operator==(Inout::Output_applicator,Inout::Output_applicator){ return true; }

bool operator==(Inout a,Inout b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Inout a,Inout b){ return !(a==b); }

Inout::Input Inout::Input_reader::operator()(Robot_inputs)const{ return {}; }
Robot_inputs Inout::Input_reader::operator()(Robot_inputs r,Inout::Input)const{ return r; }

Inout::Output Inout::Output_applicator::operator()(Robot_outputs r)const{
	return (r.relay[INOUT_ADDRESS]==Relay_output::_01? Inout::Output::OUT : (r.relay[INOUT_ADDRESS]==Relay_output::_10? Inout::Output::IN : Inout::Output::STOP));
}

Robot_outputs Inout::Output_applicator::operator()(Robot_outputs r,Inout::Output out)const{
	r.relay[INOUT_ADDRESS]=[&]{
		switch(out){
			case Inout::Output::IN: return Relay_output::_10;
			case Inout::Output::OUT: return Relay_output::_01;
			case Inout::Output::STOP: return Relay_output::_00;
			default: assert(0);
		}
	}();
	return r;
}

Inout::Status_detail Inout::Estimator::get()const{ return {}; }
void Inout::Estimator::update(Time,Inout::Input,Inout::Output){} 

std::set<Inout::Input> examples(Inout::Input*){ return {{}}; }
std::set<Inout::Goal> examples(Inout::Goal*){ return {Inout::Goal::IN,Inout::Goal::STOP,Inout::Goal::OUT}; }
std::set<Inout::Status_detail> examples(Inout::Status_detail*){ return {{}}; }

Inout::Output control(Inout::Status_detail,Inout::Goal goal){
	switch(goal){
		case Inout::Goal::IN: return Inout::Output::IN;
		case Inout::Goal::OUT: return Inout::Output::OUT;
		default: return Inout::Output::STOP;
	}
	assert(0);
}
Inout::Status status(Inout::Status_detail a){ return a; }
bool ready(Inout::Status,Inout::Goal){ return true; }

#ifdef INOUT_TEST
#include "formal.h"

int main(){
	Inout a;
	tester(a);
}

#endif

