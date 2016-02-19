#include "winch.h"

static const int WINCH_PWM = 5;
static const float WINCH_POWER = .5;

#define nyi {cout<<"nyi: line "<<__LINE__; exit(44); }

std::set<Winch::Status> examples(Winch::Status*){
	return {{}};
}

std::set<Winch::Input> examples(Winch::Input*){
	return {{}};
}

std::set<Winch::Goal> examples(Winch::Goal*){
       return {Winch::Goal::IN, Winch::Goal::OUT, Winch::Goal::STOP};
}

std::ostream& operator<<(std::ostream& o,Winch const&){
	return o<<"Winch()";
}

std::ostream& operator<<(std::ostream& o,Winch::Goal g){
	#define X(name) if(g==Winch::Goal::name) return o<<"Winch::Goal("#name")";
	X(IN) X(OUT) X(STOP)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Winch::Status){
	return o<<"Status()";
}

std::ostream& operator<<(std::ostream& o,Winch::Input){
	return o<<"Input()";
}

bool operator<(Winch::Input,Winch::Input){
	return 0;
}

bool operator==(Winch::Input,Winch::Input){
	return 1;
}

bool operator==(Winch::Estimator,Winch::Estimator){
	return 1;
}

bool operator==(Winch,Winch){
	return 1;
}

bool operator<(Winch::Status,Winch::Status){
        return 0;
}

bool operator==(Winch::Status,Winch::Status){
        return 1;
}

Winch::Input Winch::Input_reader::operator()(Robot_inputs) const{
	return {};
}

Robot_inputs Winch::Input_reader::operator()(Robot_inputs r, Winch::Input) const{
	return r;
}

Winch::Status status(Winch::Status s){
	return s;
}

bool ready(Winch::Status,Winch::Goal){
	return 1;
}

Winch::Output control(Winch::Status,Winch::Goal goal){
	return goal;
}

void Winch::Estimator::update(Time,Winch::Input,Winch::Goal){
}

Winch::Status Winch::Estimator::get()const{
	return Winch::Status{};
}

bool operator!=(Winch a, Winch b){
	return !(a==b);
}

bool operator!=(Winch::Status a, Winch::Status b){
         return !(a==b);
}

bool operator!=(Winch::Estimator a, Winch::Estimator b){
         return !(a==b);
}

bool operator!=(Winch::Input a, Winch::Input b){
	return !(a==b);
}

Robot_outputs Winch::Output_applicator::operator()(Robot_outputs ro ,Output o)const{
	if(o==Winch::Goal::OUT) ro.pwm[WINCH_PWM]=WINCH_POWER;
	else if(o==Winch::Goal::IN) ro.pwm[WINCH_PWM]=-WINCH_POWER;
	else ro.pwm[WINCH_PWM]=0;
	return ro;
}

Winch::Goal Winch::Output_applicator::operator()(Robot_outputs ro)const{
	if(ro.pwm[WINCH_PWM]>0)	return Winch::Goal::OUT;
	if(ro.pwm[WINCH_PWM]<0)	return Winch::Goal::IN;
	return Winch::Goal::STOP;
}

#ifdef WINCH_TEST
#include "formal.h"

int main(){
	Winch w;
	tester(w);
}
#endif
