#include "climb.h"
#include <stdlib.h>

#define CLIMB_ADDRESS 2

std::ostream& operator<<(std::ostream& o,Climb::Goal a){
	#define X(name) if(a==Climb::Goal::name)return o<<"Climb::Goal("#name")";
	X(UP) X(STOP) X(DOWN)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Climb::Input){
	return o<<"Climb::Input()";
}

std::ostream& operator<<(std::ostream& o,Climb::Status_detail){
	return o<<"Climb::Status_detail()";
}

std::ostream& operator<<(std::ostream& o,Climb){
	return o<<"Climb()";
}

bool operator==(Climb::Input,Climb::Input){ return true; }
bool operator<(Climb::Input,Climb::Input){ return false; }
bool operator<(Climb::Goal a,Climb::Goal b){
	return ((b==Climb::Goal::UP && a!=b) || (b==Climb::Goal::STOP && a==Climb::Goal::DOWN));
}
bool operator==(Climb::Status_detail,Climb::Status_detail){ return true; }
bool operator<(Climb::Status_detail,Climb::Status_detail){ return false; }

Climb::Status_detail Climb::Estimator::get()const{
	return {};
}

void Climb::Estimator::update(Time,Climb::Input,Climb::Output){}

Climb::Input Climb::Input_reader::operator()(Robot_inputs)const{ return {}; }

Robot_inputs Climb::Input_reader::operator()(Robot_inputs r,Climb::Input)const{ return r; }

Climb::Output Climb::Output_applicator::operator()(Robot_outputs r)const{
	double power=r.talon_srx[CLIMB_ADDRESS].power_level;
	if(power==1)return Climb::Output::UP;
	else if(power==-1)return Climb::Output::DOWN;
	return Climb::Output::STOP;
}

Robot_outputs Climb::Output_applicator::operator()(Robot_outputs r,Climb::Output out)const{
	double power=0;
	switch(out){
		case Climb::Output::UP: 
			power=1;
			break;
		case Climb::Output::DOWN:
			power=-1;
			break;
		default:
			power=0;
	}
	r.talon_srx[CLIMB_ADDRESS].power_level=power;
	return r;
}

std::set<Climb::Input> examples(Climb::Input*){
	return {{}};
}

std::set<Climb::Goal> examples(Climb::Goal*){
	return {Climb::Goal::UP,Climb::Goal::STOP,Climb::Goal::DOWN};
}

std::set<Climb::Status_detail> examples(Climb::Status_detail*){
	return {{}};
}

Climb::Output control(Climb::Status_detail,Climb::Goal goal){
	#define X(name) if(goal==Climb::Goal::name)return Climb::Output::name;
	X(UP) X(STOP) X(DOWN)
	#undef X
	assert(0);
}

Climb::Status status(Climb::Status_detail a){
	return a;
}

bool ready(Climb::Status,Climb::Goal){
	return true;
}

#ifdef CLIMB_TEST
#include "formal.h"

int main(){
	Climb a;
	tester(a);
}
#endif
