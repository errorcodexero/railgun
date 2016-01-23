#include "collector.h"
#include <stdlib.h>

using namespace std;

#define nyi { cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }
#define COLLECTOR_ADDRESS 2

ostream& operator<<(ostream& o, Collector::Goal a){
	#define X(name) if(a==Collector::Goal::name)return o<<"Collector::Goal("#name")";
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
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

bool operator==(Collector a, Collector b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Collector a, Collector b){ return !(a==b);}

Collector::Input Collector::Input_reader::operator()(Robot_inputs)const{ return Collector::Input{};}

Robot_inputs Collector::Input_reader::operator()(Robot_inputs a, Collector::Input)const{ return a;}

Robot_outputs Collector::Output_applicator::operator()(Robot_outputs r, Collector::Output out)const{
	if(out==Collector::Output::OUT) r.pwm[COLLECTOR_ADDRESS]=1;
	else if(out==Collector::Output::OFF) r.pwm[COLLECTOR_ADDRESS]=0;
	else if(out==Collector::Output::IN) r.pwm[COLLECTOR_ADDRESS]=-1;
	return r;
}

Collector::Status_detail Collector::Estimator::get()const{ return Collector::Status_detail{};}

Collector::Output Collector::Output_applicator::operator()(Robot_outputs r)const{
	if(r.pwm[COLLECTOR_ADDRESS]==1)return Collector::Output::OUT;
	if(r.pwm[COLLECTOR_ADDRESS]==0)return Collector::Output::OFF;
	if(r.pwm[COLLECTOR_ADDRESS]==-1)return Collector::Output::IN;
	assert(0);
}
	
set<Collector::Input> examples(Collector::Input*){
	return set<Collector::Input>{Collector::Input{}};
}

set<Collector::Goal> examples(Collector::Goal*){ 
	return {Collector::Goal::OUT,Collector::Goal::OFF,Collector::Goal::IN};
}

set<Collector::Status_detail> examples(Collector::Status_detail*){ 
	return set<Collector::Status_detail>{Collector::Status_detail{}};
}

Collector::Output control(Collector::Status_detail, Collector::Goal goal){
	if(goal==Collector::Goal::OUT)return Collector::Output::OUT;
	if(goal==Collector::Goal::OFF)return Collector::Output::OFF;
	if(goal==Collector::Goal::IN)return Collector::Output::IN;
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
