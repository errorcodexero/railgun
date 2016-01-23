#include "collector.h"
#include <stdlib.h>

using namespace std;

#define nyi { cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }
#define COLLECTOR_ADDRESS 2

Collector::Goal::Goal(){}

ostream& operator<<(ostream& o, Collector::Goal::Mode a){
	#define X(name) if(a==Collector::Goal::Mode::name)return o<<""#name;
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
}

Collector::Goal::Mode Collector::Goal::mode()const{
	return mode_;
}

Collector::Goal Collector::Goal::in(){
	Collector::Goal a;
	a.mode_=Collector::Goal::Mode::IN;
	return a;
}

Collector::Goal Collector::Goal::out(){
	Collector::Goal a;
	a.mode_=Collector::Goal::Mode::OUT;
	return a;
}

Collector::Goal Collector::Goal::off(){
	Collector::Goal a;
	a.mode_=Collector::Goal::Mode::OFF;
	return a;
}

ostream& operator<<(ostream& o, Collector::Goal a){
	return o<<"Collector::Goal( mode:"<<a.mode()<<")";
}

ostream& operator<<(ostream& o, Collector::Status){ return o<<"Collector::Status()";}
ostream& operator<<(ostream& o, Collector::Input){ return o<<"Collector::Input()";}
ostream& operator<<(ostream& o, Collector){ return o<<"Collector()";}

bool operator==(Collector::Input,Collector::Input){ return 1;}
bool operator!=(Collector::Input,Collector::Input){ return 0;}
bool operator<(Collector::Input, Collector::Input){ return 0;}

bool operator<(Collector::Status_detail, Collector::Status_detail){ return 0;}
bool operator==(Collector::Status_detail, Collector::Status_detail){ return 1;}
bool operator!=(Collector::Status_detail, Collector::Status_detail){ return 0;} 

bool operator==(Collector::Input_reader,Collector::Input_reader){ return 1;}
bool operator<(Collector::Input_reader, Collector::Input_reader){ return 0;}

bool operator==(Collector::Estimator, Collector::Estimator){ return 1;}
bool operator!=(Collector::Estimator, Collector::Estimator){ return 0;}

bool operator==(Collector::Output_applicator,Collector::Output_applicator){return 1;}

bool operator==(Collector::Goal b,Collector::Goal a){ return a.mode()<b.mode(); }
bool operator!=(Collector::Goal b,Collector::Goal a){ return !(a==b); }
bool operator<(Collector::Goal a, Collector::Goal b){ return a.mode()<b.mode(); }

bool operator==(Collector a, Collector b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Collector a, Collector b){ return !(a==b);}

Collector::Input Collector::Input_reader::operator()(Robot_inputs)const{ return Collector::Input{};}

Robot_inputs Collector::Input_reader::operator()(Robot_inputs a, Collector::Input)const{ return a;}

Robot_outputs Collector::Output_applicator::operator()(Robot_outputs r, Collector::Output out)const{
	if(out.mode()==Collector::Output::Mode::OUT) r.pwm[COLLECTOR_ADDRESS]=1;
	else if(out.mode()==Collector::Output::Mode::OFF) r.pwm[COLLECTOR_ADDRESS]=0;
	else if(out.mode()==Collector::Output::Mode::IN) r.pwm[COLLECTOR_ADDRESS]=-1;
	return r;
}

Collector::Status_detail Collector::Estimator::get()const{ return Collector::Status_detail{};}

Collector::Output Collector::Output_applicator::operator()(Robot_outputs r)const{
	if(r.pwm[COLLECTOR_ADDRESS]==1)return Collector::Output::out();
	if(r.pwm[COLLECTOR_ADDRESS]==0)return Collector::Output::off();
	if(r.pwm[COLLECTOR_ADDRESS]==-1)return Collector::Output::in();
	assert(0);
}
	
set<Collector::Input> examples(Collector::Input*){
	return set<Collector::Input>{Collector::Input{}};
}

set<Collector::Goal> examples(Collector::Goal*){ 
	return {Collector::Goal::out(),Collector::Goal::off(),Collector::Goal::in()};
}

set<Collector::Status_detail> examples(Collector::Status_detail*){ 
	return set<Collector::Status_detail>{Collector::Status_detail{}};
}

Collector::Output control(Collector::Status_detail, Collector::Goal goal){
	if(goal.mode()==Collector::Goal::Mode::OUT)return Collector::Output::out();
	if(goal.mode()==Collector::Goal::Mode::OFF)return Collector::Output::off();
	if(goal.mode()==Collector::Goal::Mode::IN)return Collector::Output::in();
	assert(0);
}

Collector::Status status(Collector::Status_detail a){ return a;}

bool ready(Collector::Status, Collector::Goal){ return 1;}


#ifdef COLLECTOR_TEST
#include "formal.h"

int main(){
	Collector a;
	tester(a);
}

#endif
