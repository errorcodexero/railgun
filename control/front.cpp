#include "front.h"
#include <stdlib.h>

using namespace std;

#define nyi { cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }
#define FRONT_ADDRESS 3
#ifndef BALL_SENSOR_ADDRESS
#define BALL_SENSOR_ADDRESS 6
#endif
#define FRONT_SPEED 1

ostream& operator<<(ostream& o, Front::Goal a){
	#define X(name) if(a==Front::Goal::name)return o<<"Front::Goal("#name")";
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Front::Status){ return o<<"Front::Status()";}
ostream& operator<<(ostream& o, Front::Input){ return o<<"Front::Input()";}
ostream& operator<<(ostream& o, Front){ return o<<"Front()";}

bool operator==(Front::Input, Front::Input){ return true;}
bool operator!=(Front::Input a, Front::Input b){ return !(a==b);}
bool operator<(Front::Input, Front::Input){ return false;}

bool operator<(Front::Status_detail, Front::Status_detail){ return false;}
bool operator==(Front::Status_detail, Front::Status_detail){ return true;}
bool operator!=(Front::Status_detail a, Front::Status_detail b){ return !(a==b);} 

bool operator==(Front::Input_reader,Front::Input_reader){ return 1;}
bool operator<(Front::Input_reader, Front::Input_reader){ return 0;}

bool operator==(Front::Estimator, Front::Estimator){ return 1;}
bool operator!=(Front::Estimator, Front::Estimator){ return 0;}

bool operator==(Front::Output_applicator, Front::Output_applicator){return 1;}

bool operator==(Front a, Front b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Front a, Front b){ return !(a==b);}

Front::Input Front::Input_reader::operator()(Robot_inputs)const{ return {}; }

Robot_inputs Front::Input_reader::operator()(Robot_inputs a, Front::Input)const{ return a; }

Robot_outputs Front::Output_applicator::operator()(Robot_outputs r, Front::Output out)const{
	/*r.relay[FRONT_ADDRESS]=[&]{
		switch(out){
			case Front::Output::OUT: return Relay_output::_01;
			case Front::Output::IN: return Relay_output::_10;
			default: return Relay_output::_00;
		}
	}();*/
	if(out==Front::Output::OUT) r.pwm[FRONT_ADDRESS]=FRONT_SPEED;
	else if(out==Front::Output::OFF) r.pwm[FRONT_ADDRESS]=0;
	else if(out==Front::Output::IN) r.pwm[FRONT_ADDRESS]=-FRONT_SPEED;
	return r;
}

Front::Status_detail Front::Estimator::get()const{ return Front::Status_detail{};}

Front::Output Front::Output_applicator::operator()(Robot_outputs r)const{
	//return (r.relay[FRONT_ADDRESS]==Relay_output::_01? Front::Output::OUT : (r.relay[FRONT_ADDRESS]==Relay_output::_10? Front::Output::IN : Front::Output::OFF));
	if(r.pwm[FRONT_ADDRESS]==FRONT_SPEED)return Front::Output::OUT;
	if(r.pwm[FRONT_ADDRESS]==0)return Front::Output::OFF;
	if(r.pwm[FRONT_ADDRESS]==-FRONT_SPEED)return Front::Output::IN;
	assert(0);
}
	
set<Front::Input> examples(Front::Input*){ return {{}}; }

set<Front::Goal> examples(Front::Goal*){ 
	return {Front::Goal::OUT,Front::Goal::OFF,Front::Goal::IN};
}

set<Front::Status_detail> examples(Front::Status_detail*){ return {{}}; }

Front::Output control(Front::Status_detail, Front::Goal goal){
	if(goal==Front::Goal::OUT)return Front::Output::OUT;
	if(goal==Front::Goal::OFF)return Front::Output::OFF;
	if(goal==Front::Goal::IN)return Front::Output::IN;
	assert(0);
}

Front::Status status(Front::Status_detail a){ return a;}

bool ready(Front::Status, Front::Goal){ return 1;}


#ifdef FRONT_TEST
#include "formal.h"

int main(){
	Front a;
	tester(a);
}

#endif
