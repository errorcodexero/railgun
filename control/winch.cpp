#include "winch.h"

using namespace std;

#define nyi {cout<<"nyi: line "<<__LINE__; exit(44); }

std::set<Winch::Status> examples(Winch::Status*){
	set<Winch::Status> x;
	x.insert(Winch::Status{});
	return x;
}

std::set<Winch::Goal> examples(Winch::Goal*){
        set<Winch::Goal> x;
	x.insert(Winch::Goal::IN);
	x.insert(Winch::Goal::OUT);
	x.insert(Winch::Goal::STOP);
        return x;
}

std::ostream& operator<<(std::ostream& o,Winch const&){
	return o<<"Winch()";
}

std::ostream& operator<<(std::ostream& o,Winch::Goal g){
        o<<"Goal(";
	if(g==Winch::Goal::IN)return o<<"IN)";
	else if(g==Winch::Goal::OUT)return o<<"OUT)";
	else if(g==Winch::Goal::STOP)return o<<"STOP)";
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Winch::Status){
	return o<<"Status()";
}

bool operator<(Winch::Status,Winch::Status){
        return 0;
}

bool operator==(Winch::Status,Winch::Status){
        return 1;
}

Winch::Status Winch::Input_reader::operator()(Robot_inputs) const{
	return Winch::Status{};
}

Robot_inputs Winch::Input_reader::operator()(Robot_inputs ri ,Winch::Status) const{
	return ri; 
}

Winch::Status status(Winch::Status x){
	return x;
}

bool ready(Winch::Status,Winch::Goal){
	return 1;
}

Winch::Output control(Winch::Status,Winch::Goal goal){
	return goal;
}

void Winch::Estimator::update(Time,Winch::Status,Winch::Goal){
}

Winch::Status Winch::Estimator::get()const{
	return Winch::Status{};
}

bool operator!=(Winch,Winch){
	return 0;
}

bool operator!=(Winch::Status,Winch::Status){
         return 0;
}

bool operator!=(Winch::Estimator,Winch::Estimator){
         return 0;
}

static const int WINCH_PWM = 5;
static const float WINCH_POWER = .5;

Robot_outputs Winch::Output_applicator::operator()(Robot_outputs ro ,Output o)const{
	if(o==Winch::Goal::OUT)ro.pwm[WINCH_PWM]=WINCH_POWER;
	else if(o==Winch::Goal::IN)ro.pwm[WINCH_PWM]=-WINCH_POWER;
	else ro.pwm[WINCH_PWM]=0;
	return ro;
}

Winch::Goal Winch::Output_applicator::operator()(Robot_outputs ro)const{
	if(ro.pwm[WINCH_PWM]>0)
		return Winch::Goal::OUT;
	if(ro.pwm[WINCH_PWM]<0)
		return Winch::Goal::IN;
	return Winch::Goal::STOP;
}

#ifdef WINCH_TEST
#include "formal.h"

int main(){
	Winch w;
	tester(w);
}
#endif
