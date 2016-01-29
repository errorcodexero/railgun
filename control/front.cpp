#include "front.h"
#include <stdlib.h>

using namespace std;

#define nyi { cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }
#define FRONT_ADDRESS 3
#define FRONT_SPEED 1

Front::Status_detail::Status_detail():has_ball(0){}

ostream& operator<<(ostream& o, Front::Goal a){
	#define X(name) if(a==Front::Goal::name)return o<<"Front::Goal("#name")";
	X(IN) X(OUT) X(OFF)
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o, Front::Status a){ return o<<"Front::Status( has_ball:"<<a.has_ball<<")";}
ostream& operator<<(ostream& o, Front::Input a){ return o<<"Front::Input( has_ball:"<<a.has_ball<<")";}
ostream& operator<<(ostream& o, Front){ return o<<"Front()";}

bool operator==(Front::Input a, Front::Input b){ return a.has_ball==b.has_ball;}
bool operator!=(Front::Input a, Front::Input b){ return !(a==b);}
bool operator<(Front::Input a, Front::Input b){ return a.has_ball<b.has_ball;}

bool operator<(Front::Status_detail a, Front::Status_detail b){ return a.has_ball<b.has_ball;}
bool operator==(Front::Status_detail a, Front::Status_detail b){ return a.has_ball==b.has_ball;}
bool operator!=(Front::Status_detail a, Front::Status_detail b){ return !(a==b);} 

bool operator==(Front::Input_reader,Front::Input_reader){ return 1;}
bool operator<(Front::Input_reader, Front::Input_reader){ return 0;}

bool operator==(Front::Estimator, Front::Estimator){ return 1;}
bool operator!=(Front::Estimator, Front::Estimator){ return 0;}

bool operator==(Front::Output_applicator, Front::Output_applicator){return 1;}

bool operator==(Front a, Front b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator);}
bool operator!=(Front a, Front b){ return !(a==b);}

Front::Input Front::Input_reader::operator()(Robot_inputs r)const{
	return (r.digital_io.in[6]==Digital_in::_1)? Front::Input{1} : Front::Input{0};
}

Robot_inputs Front::Input_reader::operator()(Robot_inputs a, Front::Input in)const{
	a.digital_io.in[6]=in.has_ball? Digital_in::_1 : Digital_in::_0;
	return a;
}

Robot_outputs Front::Output_applicator::operator()(Robot_outputs r, Front::Output out)const{
	if(out==Front::Output::OUT) r.pwm[FRONT_ADDRESS]=FRONT_SPEED;
	else if(out==Front::Output::OFF) r.pwm[FRONT_ADDRESS]=0;
	else if(out==Front::Output::IN) r.pwm[FRONT_ADDRESS]=-FRONT_SPEED;
	return r;
}

Front::Status_detail Front::Estimator::get()const{ return Front::Status_detail{};}

Front::Output Front::Output_applicator::operator()(Robot_outputs r)const{
	if(r.pwm[FRONT_ADDRESS]==FRONT_SPEED)return Front::Output::OUT;
	if(r.pwm[FRONT_ADDRESS]==0)return Front::Output::OFF;
	if(r.pwm[FRONT_ADDRESS]==-FRONT_SPEED)return Front::Output::IN;
	assert(0);
}
	
set<Front::Input> examples(Front::Input*){
	return set<Front::Input>{Front::Input{0},Front::Input{1}};
}

set<Front::Goal> examples(Front::Goal*){ 
	return {Front::Goal::OUT,Front::Goal::OFF,Front::Goal::IN};
}

set<Front::Status_detail> examples(Front::Status_detail*){ 
	set<Front::Status_detail> a;
	Front::Status_detail b;
	a.insert(b);
	b.has_ball=1;
	a.insert(b);
	return a;
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
