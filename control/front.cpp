#include "front.h"
#include <stdlib.h>

using namespace std;

#define FRONT_ADDRESS 2
#define FRONT_SPEED 1

ostream& operator<<(ostream& o, Front::Goal a){
	#define X(name) if(a==Front::Goal::name)return o<<"Front::Goal("#name")";
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Front::Input a){ return o<<"Front::Input(ball:"<<a.ball<<")";}
ostream& operator<<(ostream& o, Front){ return o<<"Front()";}

bool operator==(Front::Input a, Front::Input b){ return a.ball==b.ball;}
bool operator!=(Front::Input a, Front::Input b){ return !(a==b);}
bool operator<(Front::Input a, Front::Input b){ return a.ball<b.ball;}

bool operator==(Front::Input_reader,Front::Input_reader){ return 1;}
bool operator<(Front::Input_reader, Front::Input_reader){ return 0;}

bool operator==(Front::Estimator, Front::Estimator){ return 1;}
bool operator!=(Front::Estimator, Front::Estimator){ return 0;}

bool operator==(Front::Output_applicator, Front::Output_applicator){return 1;}

bool operator==(Front a, Front b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Front a, Front b){ return !(a==b);}

static const unsigned BALL_SENSOR_DIO=6;

Front::Input Front::Input_reader::operator()(Robot_inputs a)const{
	return {a.digital_io.in[BALL_SENSOR_DIO]==Digital_in::_0};
}

Robot_inputs Front::Input_reader::operator()(Robot_inputs a, Front::Input b)const{
	a.digital_io.in[BALL_SENSOR_DIO]=b.ball?Digital_in::_0:Digital_in::_1;
	return a;
}

Robot_outputs Front::Output_applicator::operator()(Robot_outputs r, Front::Output out)const{
	/*r.relay[FRONT_ADDRESS]=[&]{
		switch(out){
			case Front::Output::OUT: return Relay_output::_01;
			case Front::Output::IN: return Relay_output::_10;
			default: return Relay_output::_00;
		}
	}();*/
	if(out==Front::Output::OUT) r.pwm[FRONT_ADDRESS]=-FRONT_SPEED;
	else if(out==Front::Output::OFF) r.pwm[FRONT_ADDRESS]=0;
	else if(out==Front::Output::IN) r.pwm[FRONT_ADDRESS]=FRONT_SPEED;
	return r;
}

Front::Estimator::Estimator():input({0}){}

Front::Status_detail Front::Estimator::get()const{ return input;}

void Front::Estimator::update(Time,Front::Input input1,Front::Goal){
	input=input1;
}

Front::Output Front::Output_applicator::operator()(Robot_outputs r)const{
	//return (r.relay[FRONT_ADDRESS]==Relay_output::_01? Front::Output::OUT : (r.relay[FRONT_ADDRESS]==Relay_output::_10? Front::Output::IN : Front::Output::OFF));
	if(r.pwm[FRONT_ADDRESS]==-FRONT_SPEED)return Front::Output::OUT;
	if(r.pwm[FRONT_ADDRESS]==0)return Front::Output::OFF;
	if(r.pwm[FRONT_ADDRESS]==FRONT_SPEED)return Front::Output::IN;
	assert(0);
}
	
set<Front::Input> examples(Front::Input*){
	return {{0},{1}};
}

set<Front::Goal> examples(Front::Goal*){ 
	return {Front::Goal::OUT,Front::Goal::OFF,Front::Goal::IN};
}

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
