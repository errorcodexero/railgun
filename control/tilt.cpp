#include "tilt.h"
#include <stdlib.h>
#include <cmath>

#define VALUE_PER_DEGREE .019
#define nyi { std::cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }

Tilt::Status_detail::Status_detail(): 
	reached_ends(std::make_pair(0,0)),
	stalled(0),
	type_(Tilt::Status_detail::Type::MID),
	angle(0)
{}

Tilt::Goal::Goal():mode_(Tilt::Goal::Mode::STOP),angle_min(0),angle_target(0),angle_max(0){}

Robot_inputs Tilt::Input_reader::operator()(Robot_inputs r,Tilt::Input in)const{
	r.current[TILT_PDB_LOC]=in.current;
	r.analog[TILT_POT_LOC]=in.pot_value;
	return r;
}

Tilt::Input Tilt::Input_reader::operator()(Robot_inputs r)const{
	return {r.analog[TILT_POT_LOC],r.current[TILT_PDB_LOC]};
}

Tilt::Output Tilt::Output_applicator::operator()(Robot_outputs r)const{
	return r.pwm[TILT_ADDRESS];
}

Robot_outputs Tilt::Output_applicator::operator()(Robot_outputs r, Tilt::Output out)const{
	r.pwm[TILT_ADDRESS]=out;
	return r;
}

Tilt::Goal::Mode Tilt::Goal::mode()const{
	return mode_;
}

Tilt::Estimator::Estimator():
	last(Tilt::Status_detail::error()),
	timer_start_angle(0)
{}

std::array<double,3> Tilt::Goal::angle()const{
	assert(mode_==Tilt::Goal::Mode::GO_TO_ANGLE);
	return std::array<double,3>{angle_min,angle_target,angle_max};
}

Tilt::Goal Tilt::Goal::up(){
	Tilt::Goal a;
	a.mode_=Tilt::Goal::Mode::UP;
	return a;
}

Tilt::Goal Tilt::Goal::go_to_angle(std::array<double,3> angles){
	assert(angles[0]>=((TILT_POT_TOP-TILT_POT_OFFSET)/VALUE_PER_DEGREE) && angles[2]<=((TILT_POT_BOT-TILT_POT_OFFSET)/VALUE_PER_DEGREE));
	Tilt::Goal a;
	a.mode_=Tilt::Goal::Mode::GO_TO_ANGLE;
	a.angle_min=angles[0];
	a.angle_target=angles[1];
	a.angle_max=angles[2];
	return a;
}

Tilt::Goal Tilt::Goal::down(){
	Tilt::Goal a;
	a.mode_=Tilt::Goal::Mode::DOWN;
	return a;
}

Tilt::Goal Tilt::Goal::stop(){
	Tilt::Goal a;
	a.mode_=Tilt::Goal::Mode::STOP;
	return a;
}

Tilt::Status_detail::Type Tilt::Status_detail::type()const{
	return type_;
}

double Tilt::Status_detail::get_angle()const{
	return angle;
}

Tilt::Status_detail Tilt::Status_detail::top(){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::TOP;
	return a;
}

Tilt::Status_detail Tilt::Status_detail::mid(double d){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::MID;
	a.angle=d;
	return a;
}

Tilt::Status_detail Tilt::Status_detail::bottom(){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::BOTTOM;
	return a;
}

Tilt::Status_detail Tilt::Status_detail::error(){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::ERRORS;
	return a;
}

std::ostream& operator<<(std::ostream& o, Tilt::Status_detail::Type a){
	#define X(name) if(a==Tilt::Status_detail::Type::name) return o<<""#name;
	X(ERRORS) X(TOP) X(BOTTOM) X(MID)
	#undef X
	nyi
}

std::ostream& operator<<(std::ostream& o, Tilt::Goal::Mode a){
	#define X(name) if(a==Tilt::Goal::Mode::name) return o<<""#name;
	X(UP) X(DOWN) X(GO_TO_ANGLE) X(STOP)
	#undef X
	nyi
}

std::ostream& operator<<(std::ostream& o, Tilt::Status_detail a){ 
	o<<"Tilt::Status_detail(";
	o<<" stalled:"<<a.stalled;
	o<<" reached_ends:"<<a.reached_ends;
	o<<" type:"<<a.type();
	if(a.type()==Tilt::Status_detail::Type::MID){
		o<<" "<<a.get_angle();
	}
	return o<<")";
}

std::ostream& operator<<(std::ostream& o, Tilt::Goal a){ 
	o<<"Tilt::Goal(";
	o<<" mode:"<<a.mode();
	if(a.mode()==Tilt::Goal::Mode::GO_TO_ANGLE) o<<" angles:"<<a.angle();
	return o<<")";
}

std::ostream& operator<<(std::ostream& o, Tilt::Output_applicator){ return o<<"Tilt::Output_applicator()";} 
std::ostream& operator<<(std::ostream& o, Tilt::Input_reader){ return o<<"Tilt::Input_reader()";}
std::ostream& operator<<(std::ostream& o, Tilt::Estimator){ return o<<"Tilt::Estimator()";} 
 
std::ostream& operator<<(std::ostream& o, Tilt a){
	o<<"Tilt(";
	o<<" "<<a.output_applicator;
	o<<" "<<a.input_reader;
	o<<" "<<a.estimator;
	return o<<")";
}

#define CMP(name) if(a.name<b.name) return 1; if(b.name<a.name) return 0;

bool operator==(Tilt::Input const& a,Tilt::Input const& b){
	return a.pot_value==b.pot_value && a.current==b.current;	
}

bool operator!=(Tilt::Input const& a,Tilt::Input const& b){ return !(a==b); }

bool operator<(Tilt::Input const& a,Tilt::Input const& b){ return a.pot_value<b.pot_value; }

std::ostream& operator<<(std::ostream& o,Tilt::Input const& a){ return o<<"Tilt::Input( pot_value:"<<a.pot_value<<" current:"<<a.current<<")"; }

bool operator<(Tilt::Status_detail a, Tilt::Status_detail b){
	CMP(type())
	if(a.type()==Tilt::Status_detail::Type::MID)return a.get_angle()<b.get_angle();
	CMP(reached_ends)
	return !a.stalled && b.stalled;
}

bool operator==(Tilt::Status_detail a,Tilt::Status_detail b){
	if(a.type()!=b.type()) return 0;
	return ((a.type()!=Tilt::Status_detail::Type::MID || a.get_angle()==b.get_angle()) && a.reached_ends==b.reached_ends && a.stalled==b.stalled);
}

bool operator!=(Tilt::Status_detail a,Tilt::Status_detail b){ return !(a==b); }

bool operator==(Tilt::Goal a, Tilt::Goal b){ return (a.mode()==b.mode() && a.angle()==b.angle()); }
bool operator!=(Tilt::Goal a, Tilt::Goal b){ return !(a==b); }

bool operator<(Tilt::Goal a, Tilt::Goal b){
	if(a.mode()<b.mode())return true;
	if(b.mode()>a.mode())return false;
	if(a.mode()==Tilt::Goal::Mode::GO_TO_ANGLE && a.mode()==b.mode())return a.angle()<b.angle();
	return false;
}

bool operator==(Tilt::Output_applicator,Tilt::Output_applicator){ return true; }
bool operator==(Tilt::Input_reader,Tilt::Input_reader){ return true; }
bool operator==(Tilt::Estimator a,Tilt::Estimator b){ return a.last==b.last; }
bool operator!=(Tilt::Estimator a,Tilt::Estimator b){ return !(a==b); }

bool operator==(Tilt a, Tilt b){ return (a.output_applicator==b.output_applicator && a.input_reader==b.input_reader && a.estimator==b.estimator); }
bool operator!=(Tilt a, Tilt b){ return !(a==b); }

std::set<Tilt::Input> examples(Tilt::Input*){ 
	return {{.19,0},{.69,0},{1.19,0},{1.69,0},{2.19,0}};
}
std::set<Tilt::Goal> examples(Tilt::Goal*){
	return {
		Tilt::Goal::up(),
		Tilt::Goal::down(),
		Tilt::Goal::go_to_angle(std::array<double,3>{0,0,0}),
		Tilt::Goal::stop()
	};
}

std::set<Tilt::Status_detail> examples(Tilt::Status_detail*){
	return {
		Tilt::Status_detail::top(),
		Tilt::Status_detail::mid(0),
		Tilt::Status_detail::bottom(),
		Tilt::Status_detail::error()
	};
}

std::set<Tilt::Output> examples(Tilt::Output*){ 
	return {-1,0,1};
}
Tilt::Output control(Tilt::Status_detail status, Tilt::Goal goal){
	switch(goal.mode()){
		case Tilt::Goal::Mode::UP:
			switch(status.type()){
				case Tilt::Status_detail::Type::TOP:
				case Tilt::Status_detail::Type::ERRORS:
					return 0;
				case Tilt::Status_detail::Type::BOTTOM:
				case Tilt::Status_detail::Type::MID:
					return 1;
				default: assert(0);
			}
		case Tilt::Goal::Mode::DOWN:
			switch(status.type()){
				case Tilt::Status_detail::Type::BOTTOM:
				case Tilt::Status_detail::Type::ERRORS:
					return 0;
				case Tilt::Status_detail::Type::TOP:
				case Tilt::Status_detail::Type::MID:
					return -1;
				default: assert(0);
			}
		case Tilt::Goal::Mode::GO_TO_ANGLE:
			{
				const double POWER=1;
				const double SLOW=(POWER/5);
				switch (status.type()) {
					case Tilt::Status_detail::Type::MID:
						{
							double error=goal.angle()[1]-status.get_angle();
							double desired_power=error*SLOW;
							if(desired_power>POWER)return POWER;
							if(desired_power<-POWER)return -POWER;
							return desired_power;
						}
					case Tilt::Status_detail::Type::TOP:
						return -POWER;
					case Tilt::Status_detail::Type::BOTTOM:
						return POWER;
					case Tilt::Status_detail::Type::ERRORS:
						return 0.0;
					default:
						assert(0);
				}
			}
		case Tilt::Goal::Mode::STOP: return 0;
		default: assert(0);
	}
}

Tilt::Status status(Tilt::Status_detail a){
	return a;
}

bool ready(Tilt::Status status, Tilt::Goal goal){
	switch(goal.mode()){
		case Tilt::Goal::Mode::UP: return status.type()==Tilt::Status_detail::Type::TOP;
		case Tilt::Goal::Mode::GO_TO_ANGLE: return (status.get_angle()>goal.angle()[0] && status.get_angle()<goal.angle()[2]);
		case Tilt::Goal::Mode::DOWN: return status.type()==Tilt::Status_detail::Type::BOTTOM;
		case Tilt::Goal::Mode::STOP: return 1;
		default: nyi
	}
}

template<typename T>
bool in_range(T a, T b, T c){//returns if a is in a range of +/- c from b
	return a>(b-c) && a<(b+c);
}

void Tilt::Estimator::update(Time time, Tilt::Input in, Tilt::Output) {
	float angle=(in.pot_value-TILT_POT_TOP)/VALUE_PER_DEGREE;
	stall_timer.update(time,true);
	if(stall_timer.done()) last.stalled=true;
	if(in.current<10 || fabs(angle-timer_start_angle)<1){//Assumed current for now
		last.stalled=0;
		stall_timer.set(1);
		timer_start_angle=angle;
	}
	const float ALLOWED_TOLERANCE=.03;
	if(in_range(in.pot_value,(float)TILT_POT_TOP,ALLOWED_TOLERANCE)){
		if(in_range(in.pot_value,(float)TILT_POT_BOT,ALLOWED_TOLERANCE)){
			last=Tilt::Status_detail::error();
		} else{
			last=Tilt::Status_detail::top();                       
		}
	} else{
		if(in_range(in.pot_value,(float)TILT_POT_BOT,ALLOWED_TOLERANCE)){
			last=Tilt::Status_detail::bottom();
		} else{
			last=Tilt::Status_detail::mid(angle);
		}
	}
}

Tilt::Status_detail Tilt::Estimator::get()const {
	return last;
}

#ifdef TILT_TEST
#include "formal.h"

int main(){
	Tilt a;
	tester(a);	
}
#endif
