#include "shooter.h"
#include "stdlib.h"

#define PREP_RPM 1000
#define HALL_EFFECT_LOC 5

std::ostream& operator<<(std::ostream& o,Shooter::Goal goal){
	#define X(NAME) if(goal==Shooter::Goal::NAME) return o<<"Shooter::Goal("#NAME")";
	GOALS
	#undef X
	assert(0);
}
std::ostream& operator<<(std::ostream& o,Shooter::Input a){ return o<<"Shooter::Input( passing:"<<a.passing<<")"; }
std::ostream& operator<<(std::ostream& o,Shooter::Status_detail a){	
	return o<<"Shooter::Status_detail( count_rev"<<a.count_rev<<" rpm:"<<a.rpm<<")";
}
std::ostream& operator<<(std::ostream& o,Shooter){ return o<<"Shooter()"; }

bool operator==(Shooter::Input a,Shooter::Input b){ return a.passing==b.passing; }
bool operator!=(Shooter::Input a,Shooter::Input b){ return !(a==b); }
bool operator<(Shooter::Input a,Shooter::Input b){ return !a.passing && b.passing; }

bool operator<(Shooter::Status_detail a,Shooter::Status_detail b){
	if(a.count_rev<b.count_rev) return true;
	if(a.count_rev>b.count_rev) return false;
	return a.rpm<b.rpm;
}
bool operator==(Shooter::Status_detail a,Shooter::Status_detail b){ return (a.count_rev==b.count_rev && a.rpm==b.rpm); }
bool operator!=(Shooter::Status_detail a,Shooter::Status_detail b){ return !(a==b); }

bool operator==(Shooter::Input_reader,Shooter::Input_reader){ return true; }
bool operator<(Shooter::Input_reader,Shooter::Input_reader){ return false; }

bool operator==(Shooter::Estimator,Shooter::Estimator){ return true; }
bool operator!=(Shooter::Estimator,Shooter::Estimator){ return false; }

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator){ return true; }

bool operator==(Shooter a,Shooter b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Shooter a,Shooter b){ return !(a==b); }

Shooter::Input Shooter::Input_reader::operator()(Robot_inputs r)const{
	return {r.digital_io.in[HALL_EFFECT_LOC]==Digital_in::_1};
}
Robot_inputs Shooter::Input_reader::operator()(Robot_inputs r,Shooter::Input in)const{
	r.digital_io.in[HALL_EFFECT_LOC]=(in.passing? Digital_in::_1 : Digital_in::_0);
	return r;
}

Shooter::Output Shooter::Output_applicator::operator()(Robot_outputs)const{ return {}; }
Robot_outputs Shooter::Output_applicator::operator()(Robot_outputs r,Shooter::Output)const{ return r; }

Shooter::Status_detail Shooter::Estimator::get()const{ return {}; }
void Shooter::Estimator::update(Time,Shooter::Input,Shooter::Output){} 

std::set<Shooter::Input> examples(Shooter::Input*){
	return {{true},{false}}; 
}
std::set<Shooter::Goal> examples(Shooter::Goal*){
	std::set<Shooter::Goal> s;
	#define X(NAME) s.insert(Shooter::Goal::NAME);
	GOALS
	#undef X
	return s;
}
std::set<Shooter::Status_detail> examples(Shooter::Status_detail*){
	return {
		{0,0}
	};
}
std::set<Shooter::Status> examples(Shooter::Status*){
	return {{0}};
}

Shooter::Output control(Shooter::Status_detail status,Shooter::Goal goal){
	switch(goal){
		#define X(NAME) case Shooter::Goal::NAME: return Shooter::Output::NAME;
		GOALS
		#undef X
		default: assert(0);
	}
}
Shooter::Status status(Shooter::Status_detail a){
	return a.rpm;
}

bool ready(Shooter::Status status,Shooter::Goal goal){
	switch(goal){
		case Shooter::Goal::PREP: return status==PREP_RPM;
		case Shooter::Goal::OFF: return status==0;
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

