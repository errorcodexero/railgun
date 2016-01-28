#include "climb.h"
#include <stdlib.h>

#define CLIMB_ADDRESS 2

Climb::Status_detail::Type Climb::Status_detail::type()const{ return type_; }

Climb::Status_detail Climb::Status_detail::top(){
	Climb::Status_detail a;
	a.type_=Climb::Status_detail::Type::TOP;
	return a;
}

Climb::Status_detail Climb::Status_detail::bottom(){
	Climb::Status_detail a;
	a.type_=Climb::Status_detail::Type::BOTTOM;
	return a;
}

Climb::Status_detail Climb::Status_detail::mid(){
	Climb::Status_detail a;
	a.type_=Climb::Status_detail::Type::MID;
	return a;
}

Climb::Status_detail Climb::Status_detail::error(){
	Climb::Status_detail a;
	a.type_=Climb::Status_detail::Type::ERRORS;
	return a;
}

Climb::Status_detail::Status_detail():reached_ends(std::make_pair(false,false)),type_(Climb::Status_detail::Type::MID){}

Climb::Estimator::Estimator():last(Status_detail::error()){}

std::ostream& operator<<(std::ostream& o,Climb::Status_detail::Type a){
	#define X(name) if(a==Climb::Status_detail::Type::name)return o<<"Climb::Status_detail::Type("#name")";
	X(TOP) X(BOTTOM) X(MID) X(ERRORS)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Climb::Goal a){
	#define X(name) if(a==Climb::Goal::name)return o<<"Climb::Goal("#name")";
	X(UP) X(STOP) X(DOWN)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Climb::Input a){
	return o<<"Climb::Input( top:"<<a.top<<" bottom:"<<a.bottom<<")";
}

std::ostream& operator<<(std::ostream& o,Climb::Status_detail a){
	return o<<"Climb::Status_detail( type:"<<a.type()<<" reached_ends:"<<a.reached_ends<<")";
}

std::ostream& operator<<(std::ostream& o,Climb){
	return o<<"Climb()";
}

bool operator==(Climb::Input a,Climb::Input b){ return (a.top==b.top && a.bottom==b.bottom); }
bool operator!=(Climb::Input a,Climb::Input b){ return !(a==b); }
bool operator<(Climb::Input a,Climb::Input b){ return (a.top<b.top && a.bottom<b.bottom); }

bool operator<(Climb::Goal a,Climb::Goal b){
	return ((b==Climb::Goal::UP && a!=b) || (b==Climb::Goal::STOP && a==Climb::Goal::DOWN));
}
bool operator==(Climb::Status_detail a,Climb::Status_detail b){ return (a.type()==b.type() && a.reached_ends==b.reached_ends); }
bool operator!=(Climb::Status_detail a,Climb::Status_detail b){ return !(a==b); }
bool operator<(Climb::Status_detail a,Climb::Status_detail b){ return (a.type()<b.type() && a.reached_ends<b.reached_ends); }

bool operator==(Climb::Input_reader,Climb::Input_reader){ return true; }
bool operator==(Climb::Output_applicator,Climb::Output_applicator){ return true; }

bool operator==(Climb::Estimator a,Climb::Estimator b){ return (a.last==b.last); }
bool operator!=(Climb::Estimator a,Climb::Estimator b){ return !(a==b); }

bool operator==(Climb a,Climb b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Climb a,Climb b){ return !(a==b); }

Climb::Status_detail Climb::Estimator::get()const{
	return last;
}

void Climb::Estimator::update(Time,Climb::Input,Climb::Output){}

Climb::Input Climb::Input_reader::operator()(Robot_inputs r)const{
	return {r.digital_io.in[4]==Digital_in::_1,r.digital_io.in[5]==Digital_in::_1};
}

Robot_inputs Climb::Input_reader::operator()(Robot_inputs r,Climb::Input in)const{
	r.digital_io.in[4]=in.top? Digital_in::_1 : Digital_in::_0;
	r.digital_io.in[5]=in.bottom? Digital_in::_1 : Digital_in::_0;
	return r;
}

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
	return {{0,0},{1,0},{0,1},{1,1}};
}

std::set<Climb::Goal> examples(Climb::Goal*){
	return {Climb::Goal::UP,Climb::Goal::STOP,Climb::Goal::DOWN};
}

std::set<Climb::Status_detail> examples(Climb::Status_detail*){
	std::set<Climb::Status_detail> a;
	a.insert(Climb::Status_detail::mid());
	a.insert(Climb::Status_detail::top());
	a.insert(Climb::Status_detail::bottom());
	a.insert(Climb::Status_detail::error());
	return a;
}

Climb::Output control(Climb::Status_detail status,Climb::Goal goal){
	switch(goal){
		case Climb::Goal::UP: return status.type()==Climb::Status_detail::Type::TOP? Climb::Output::STOP : Climb::Output::UP;
		case Climb::Goal::DOWN: return status.type()==Climb::Status_detail::Type::BOTTOM? Climb::Output::STOP : Climb::Output::DOWN;	
		default: return Climb::Output::STOP;
	}
}

Climb::Status status(Climb::Status_detail a){
	return a;
}

bool ready(Climb::Status status,Climb::Goal goal){
	switch(goal){
		case Climb::Goal::UP: return status.type()==Climb::Status::Type::TOP;
		case Climb::Goal::DOWN: return status.type()==Climb::Status::Type::BOTTOM;
		case Climb::Goal::STOP: return true;			
		default: assert(0);
	}
}

#ifdef CLIMB_TEST
#include "formal.h"

int main(){
	Climb a;
	tester(a);
}
#endif