#include "collector.h"

using namespace std;

#define COLLECTOR_ADDRESS 2

ostream& operator<<(ostream& o, Collector::Goal a){
	if(a==Collector::Goal::OFF) return o<<"Collector::Goal(OFF)";
	if(a==Collector::Goal::REVERSE) return o<<"Collector::Goal(REVERSE)";
	if(a==Collector::Goal::FORWARD) return o<<"Collector::Goal(FORWARD)";
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
bool operator<(Collector::Input_reader, Collector::Input_reader){ return 0;}
bool operator==(Collector::Estimator, Collector::Estimator){ return 1;}
bool operator!=(Collector::Estimator, Collector::Estimator){ return 0;}
bool operator==(Collector::Input_reader,Collector::Input_reader){ return 1;}
bool operator==(Collector::Output_applicator,Collector::Output_applicator){return 1;}
bool operator==(Collector a, Collector b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Collector a, Collector b){ return !(a==b);}

Collector::Input Collector::Input_reader::operator()(Robot_inputs)const{ return Collector::Input{};}

Robot_inputs Collector::Input_reader::operator()(Robot_inputs a, Collector::Input)const{ return a;}

Robot_outputs Collector::Output_applicator::operator()(Robot_outputs r, Collector::Output out)const{
	r.pwm[COLLECTOR_ADDRESS]=out;
	return r;
}

Collector::Status_detail Collector::Estimator::get()const{ return Collector::Status_detail{};}

Collector::Output Collector::Output_applicator::operator()(Robot_outputs r)const{
	return r.pwm[COLLECTOR_ADDRESS];
}
	
set<Collector::Input> examples(Collector::Input*){
	return set<Collector::Input>{Collector::Input{}};
}

set<Collector::Goal> examples(Collector::Goal*){ 
	return set<Collector::Goal>{Collector::Goal::FORWARD,Collector::Goal::OFF,Collector::Goal::REVERSE};
}

set<Collector::Status_detail> examples(Collector::Status_detail*){ 
	return set<Collector::Status_detail>{Collector::Status_detail{}};
}
set<Collector::Output> examples(Collector::Output*){ 
	return set<Collector::Output>{1,0,-1};
}


Collector::Output control(Collector::Status_detail, Collector::Goal goal){
	if(goal==Collector::Goal::FORWARD)return Collector::Output{-1};
	if(goal==Collector::Goal::OFF)return Collector::Output{0};
	if(goal==Collector::Goal::REVERSE)return Collector::Output{1};
	assert(0);
}

Collector::Status status(Collector::Status_detail){ return Collector::Status{};}

bool ready(Collector::Status, Collector::Goal){return 1;}


#ifdef COLLECTOR_TEST
#include "formal.h"

int main(){
	Collector a;
	tester(a);
}

#endif
