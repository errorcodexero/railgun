#include "winch.h"

static const int WINCH_PWM = 5;
static const float WINCH_POWER = .8;

#define nyi {cout<<"nyi: line "<<__LINE__; exit(44); }
#define LIMIT_LOC 4

Winch::Input::Input():deployed(0){}

std::set<Winch::Goal> examples(Winch::Goal*){
       return {Winch::Goal::IN, Winch::Goal::OUT, Winch::Goal::STOP};
}

std::set<Winch::Input> examples(Winch::Input*){
	std::set<Winch::Input> s;
	Winch::Input in;
	in.deployed=true;
	s.insert(in);
	in.deployed=false;
	s.insert(in);
	return s;
}

std::ostream& operator<<(std::ostream& o,Winch::Goal g){
	#define X(name) if(g==Winch::Goal::name) return o<<"Winch::Goal("#name")";
	X(IN) X(OUT) X(STOP)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Winch::Input a){
	return o<<"Input( deployed:"<<a.deployed<<")";
}

std::ostream& operator<<(std::ostream& o,Winch const&){
	return o<<"Winch()";
}

bool operator<(Winch::Input a,Winch::Input b){
	return !a.deployed && b.deployed;
}
bool operator==(Winch::Input a,Winch::Input b){ return a.deployed==b.deployed; }
bool operator!=(Winch::Input a, Winch::Input b){ return !(a==b); }

bool operator==(Winch::Estimator,Winch::Estimator){ return 1; }
bool operator!=(Winch::Estimator a, Winch::Estimator b){ return !(a==b); }

bool operator==(Winch,Winch){ return 1; }
bool operator!=(Winch a, Winch b){ return !(a==b); }

Winch::Input Winch::Input_reader::operator()(Robot_inputs r) const{
	Winch::Input in;
	in.deployed=(r.digital_io.in[LIMIT_LOC]==Digital_in::_1);
	return in;
}

Robot_inputs Winch::Input_reader::operator()(Robot_inputs r, Winch::Input in) const{
	r.digital_io.in[LIMIT_LOC]=(in.deployed ? Digital_in::_1 : Digital_in::_0);
	return r;
}

Robot_outputs Winch::Output_applicator::operator()(Robot_outputs r, Winch::Output o)const{
	if(o==Winch::Goal::OUT) r.pwm[WINCH_PWM]=WINCH_POWER;
	else if(o==Winch::Goal::IN) r.pwm[WINCH_PWM]=-WINCH_POWER;
	else r.pwm[WINCH_PWM]=0;
	return r;
}

Winch::Goal Winch::Output_applicator::operator()(Robot_outputs r)const{
	if(r.pwm[WINCH_PWM]>0)	return Winch::Goal::OUT;
	if(r.pwm[WINCH_PWM]<0)	return Winch::Goal::IN;
	return Winch::Goal::STOP;
}

void Winch::Estimator::update(Time,Winch::Input,Winch::Goal){

}

Winch::Status Winch::Estimator::get()const{
	return Winch::Status{};
}

Winch::Output control(Winch::Status,Winch::Goal goal){
	return goal;
}

Winch::Status status(Winch::Status s){
	return s;
}

bool ready(Winch::Status status,Winch::Goal goal){
	if(goal==Winch::Goal::IN) return status.deployed;
	return 1;
}

#ifdef WINCH_TEST
#include "formal.h"
int main(){
	Winch w;
	tester(w);
}
#endif
