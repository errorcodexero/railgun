#include "drivebase.h"
#include "../util/fixVictor.h"
#include <iostream>
#include <math.h>
#include "../util/util.h"
#include "formal.h"

using namespace std;

unsigned pdb_location(Drivebase::Motor m){
	#define X(NAME,INDEX) if(m==Drivebase::NAME) return INDEX;
	//WILL NEED CORRECT VALUES
	X(LEFT1,0)
        X(LEFT2,1)
	X(RIGHT1,2)
	X(RIGHT2,3)
        #undef X
        assert(0);
        //assert(m>=0 && m<Drivebase::MOTORS);
}

Robot_inputs Drivebase::Input_reader::operator()(Robot_inputs all,Input in)const{
	for(unsigned i=0;i<MOTORS;i++){
		all.current[pdb_location((Motor)i)]=in.current[i];
	}
	auto set=[&](unsigned index,Digital_in value){
		all.digital_io.in[index]=value;
	};
	auto encoder=[&](unsigned a,unsigned b,Encoder_info e){
		set(a,e.first);
		set(b,e.second);
	};
	encoder(0,1,in.left);
	encoder(2,3,in.right);
	return all;
}

Drivebase::Input Drivebase::Input_reader::operator()(Robot_inputs in)const{
	auto encoder_info=[&](unsigned a, unsigned b){
		return make_pair(in.digital_io.in[a],in.digital_io.in[b]);
	};
	return Drivebase::Input{
		[&](){
			array<double,Drivebase::MOTORS> r;
			for(unsigned i=0;i<Drivebase::MOTORS;i++){
				Drivebase::Motor m=(Drivebase::Motor)i;
				r[i]=in.current[pdb_location(m)];
			}
			return r;
		}(),
		encoder_info(0,1),
		encoder_info(2,3)
	};
}

IMPL_STRUCT(Drivebase::Status::Status,DRIVEBASE_STATUS)
IMPL_STRUCT(Drivebase::Input::Input,DRIVEBASE_INPUT)
IMPL_STRUCT(Drivebase::Output::Output,DRIVEBASE_OUTPUT)

CMP_OPS(Drivebase::Input,DRIVEBASE_INPUT)

CMP_OPS(Drivebase::Status,DRIVEBASE_STATUS)

set<Drivebase::Status> examples(Drivebase::Status*){
	return {Drivebase::Status{
		array<Motor_check::Status,Drivebase::MOTORS>{
			Motor_check::Status::OK_,
			Motor_check::Status::OK_
		}
	}};
}

set<Drivebase::Goal> examples(Drivebase::Goal*){
	return {
		Drivebase::Goal{0,0},
		Drivebase::Goal{1,1}
	};
}

ostream& operator<<(ostream& o,Drivebase::Goal const& a){
	return o<<"Drivebase::Goal("<<a.left<<" "<<a.right<<")";
}

#define CMP(name) if(a.name<b.name) return 1; if(b.name<a.name) return 0;

bool operator<(Drivebase::Goal const& a,Drivebase::Goal const& b){
	CMP(left)
	CMP(right)
	return 0;
}

CMP_OPS(Drivebase::Output,DRIVEBASE_OUTPUT)

set<Drivebase::Output> examples(Drivebase::Output*){
	return {
		Drivebase::Output{0,0},//velToPwm(0),velToPwm(0)},
		Drivebase::Output{1,1}//velToPwm(1),velToPwm(1)}
	};
}

set<Drivebase::Input> examples(Drivebase::Input*){
	auto d=Digital_in::_0;
	auto p=make_pair(d,d);
	return {Drivebase::Input{
		{0,0},p,p
	}};
}

Drivebase::Status_detail Drivebase::Estimator::get()const{
	array<Motor_check::Status,MOTORS> a;
	for(unsigned i=0;i<a.size();i++){
		a[i]=motor_check[i].get();
	}
	return Status{a};
}

ostream& operator<<(ostream& o,Drivebase::Output_applicator){
	return o<<"output_applicator";
}

ostream& operator<<(ostream& o,Drivebase const& a){
	return o<<"Drivebase("<<a.estimator.get()<<")";
}

double get_output(Drivebase::Output out,Drivebase::Motor m){
	#define X(NAME,POSITION) if(m==Drivebase::NAME) return out.POSITION;
	X(LEFT1,l)
	X(LEFT2,l)
	X(RIGHT1,r)
	X(RIGHT2,r)
	#undef X
	assert(0);
}

void Drivebase::Estimator::update(Time now,Drivebase::Input in,Drivebase::Output out){
	for(unsigned i=0;i<MOTORS;i++){
		Drivebase::Motor m=(Drivebase::Motor)i;
		auto current=in.current[i];
		auto set_power_level=get_output(out,m);
		motor_check[i].update(now,current,set_power_level);
	}
}

Robot_outputs Drivebase::Output_applicator::operator()(Robot_outputs robot,Drivebase::Output b)const{
	robot.pwm[0]=-pwm_convert(b.l);
	robot.pwm[1]=pwm_convert(b.r);
	return robot;
}

Drivebase::Output Drivebase::Output_applicator::operator()(Robot_outputs robot)const{
	return Drivebase::Output{
		-from_pwm(robot.pwm[0]),
		from_pwm(robot.pwm[1])
	};
}

bool operator==(Drivebase::Output_applicator const&,Drivebase::Output_applicator const&){
	return 1;
}

bool operator==(Drivebase::Estimator const&,Drivebase::Estimator const&){
	return 1;
}

bool operator!=(Drivebase::Estimator const& a,Drivebase::Estimator const& b){
	return !(a==b);
}

bool operator==(Drivebase const& a,Drivebase const& b){
	return a.estimator==b.estimator && a.output_applicator==b.output_applicator;
}

bool operator!=(Drivebase const& a,Drivebase const& b){
	return !(a==b);
}

Drivebase::Output control(Drivebase::Status /*status*/,Drivebase::Goal goal){
	return Drivebase::Output{goal.left,goal.right};//{adjust_for_victor(goal.left),adjust_for_victor(goal.right)};//velToPwm(goal.left),velToPwm(goal.right)};
}

Drivebase::Status status(Drivebase::Status a){ return a; }

bool ready(Drivebase::Status,Drivebase::Goal){ return 1; }

#ifdef DRIVEBASE_TEST
int main(){
	Drivebase d;
	cout<<d.output_applicator<<"\n";
	tester(d);
}
#endif
