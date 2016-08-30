#include "sides.h"
#include <stdlib.h>

using namespace std;

#define SIDES_ADDRESS 3
#define SIDES_SPEED 1

ostream& operator<<(ostream& o, Sides::Goal a){
	#define X(name) if(a==Sides::Goal::name)return o<<"Sides::Goal("#name")";
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Sides){ return o<<"Sides()";}

bool operator==(Sides::Output_applicator, Sides::Output_applicator){return 1;}

bool operator==(Sides a, Sides b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Sides a, Sides b){ return !(a==b);}

Robot_outputs Sides::Output_applicator::operator()(Robot_outputs r, Sides::Output out)const{
	if(out==Sides::Output::OUT) r.pwm[SIDES_ADDRESS]=-SIDES_SPEED;
	else if(out==Sides::Output::OFF) r.pwm[SIDES_ADDRESS]=0;
	else if(out==Sides::Output::IN) r.pwm[SIDES_ADDRESS]=SIDES_SPEED;
	return r;
}

Sides::Output Sides::Output_applicator::operator()(Robot_outputs r)const{
	if(r.pwm[SIDES_ADDRESS]==-SIDES_SPEED)return Sides::Output::OUT;
	if(r.pwm[SIDES_ADDRESS]==0)return Sides::Output::OFF;
	if(r.pwm[SIDES_ADDRESS]==SIDES_SPEED)return Sides::Output::IN;
	assert(0);
}
	
set<Sides::Goal> examples(Sides::Goal*){ 
	return {Sides::Goal::OUT,Sides::Goal::OFF,Sides::Goal::IN};
}

Sides::Output control(Sides::Status_detail, Sides::Goal goal){
	if(goal==Sides::Goal::OUT)return Sides::Output::OUT;
	if(goal==Sides::Goal::OFF)return Sides::Output::OFF;
	if(goal==Sides::Goal::IN)return Sides::Output::IN;
	assert(0);
}

bool ready(Sides::Status, Sides::Goal){ return 1;}

#ifdef SIDES_TEST
#include "formal.h"

int main(){
	Sides a;
	tester(a);
}

#endif
