#include "sides.h"
#include <stdlib.h>

using namespace std;

#define nyi { cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }
#define SIDES_ADDRESS 2

ostream& operator<<(ostream& o, Sides::Goal a){
	#define X(name) if(a==Sides::Goal::name)return o<<"Sides::Goal("#name")";
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Sides::Status){ return o<<"Sides::Status()";}
ostream& operator<<(ostream& o, Sides::Input){ return o<<"Sides::Input()";}
ostream& operator<<(ostream& o, Sides){ return o<<"Sides()";}

bool operator==(Sides::Input, Sides::Input){ return 1;}
bool operator!=(Sides::Input, Sides::Input){ return 0;}
bool operator<(Sides::Input, Sides::Input){ return 0;}

bool operator<(Sides::Status_detail, Sides::Status_detail){ return 0;}
bool operator==(Sides::Status_detail, Sides::Status_detail){ return 1;}
bool operator!=(Sides::Status_detail, Sides::Status_detail){ return 0;} 

bool operator==(Sides::Input_reader,Sides::Input_reader){ return 1;}
bool operator<(Sides::Input_reader, Sides::Input_reader){ return 0;}

bool operator==(Sides::Estimator, Sides::Estimator){ return 1;}
bool operator!=(Sides::Estimator, Sides::Estimator){ return 0;}

bool operator==(Sides::Output_applicator, Sides::Output_applicator){return 1;}

bool operator==(Sides a, Sides b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Sides a, Sides b){ return !(a==b);}

Sides::Input Sides::Input_reader::operator()(Robot_inputs)const{ return Sides::Input{};}

Robot_inputs Sides::Input_reader::operator()(Robot_inputs a, Sides::Input)const{ return a;}

Robot_outputs Sides::Output_applicator::operator()(Robot_outputs r, Sides::Output out)const{
	if(out==Sides::Output::OUT) r.pwm[SIDES_ADDRESS]=1;
	else if(out==Sides::Output::OFF) r.pwm[SIDES_ADDRESS]=0;
	else if(out==Sides::Output::IN) r.pwm[SIDES_ADDRESS]=-1;
	return r;
}

Sides::Status_detail Sides::Estimator::get()const{ return Sides::Status_detail{};}

Sides::Output Sides::Output_applicator::operator()(Robot_outputs r)const{
	if(r.pwm[SIDES_ADDRESS]==1)return Sides::Output::OUT;
	if(r.pwm[SIDES_ADDRESS]==0)return Sides::Output::OFF;
	if(r.pwm[SIDES_ADDRESS]==-1)return Sides::Output::IN;
	assert(0);
}
	
set<Sides::Input> examples(Sides::Input*){
	return set<Sides::Input>{Sides::Input{}};
}

set<Sides::Goal> examples(Sides::Goal*){ 
	return {Sides::Goal::OUT,Sides::Goal::OFF,Sides::Goal::IN};
}

set<Sides::Status_detail> examples(Sides::Status_detail*){ 
	return set<Sides::Status_detail>{Sides::Status_detail{}};
}

Sides::Output control(Sides::Status_detail, Sides::Goal goal){
	if(goal==Sides::Goal::OUT)return Sides::Output::OUT;
	if(goal==Sides::Goal::OFF)return Sides::Output::OFF;
	if(goal==Sides::Goal::IN)return Sides::Output::IN;
	assert(0);
}

Sides::Status status(Sides::Status_detail a){ return a;}

bool ready(Sides::Status, Sides::Goal){ return 1;}


#ifdef SIDES_TEST
#include "formal.h"

int main(){
	Sides a;
	tester(a);
}

#endif