#include "tilt.h"
#include <stdlib.h>

#define nyi { std::cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }

Tilt::Status_detail::Status_detail(): 
	reached_ends(std::make_pair(0,0)),
	stalled(0),
	type_(Tilt::Status_detail::Type::MID),
	angle(0)
{}

Tilt::Goal::Goal():mode_(Tilt::Goal::Mode::STOP),angle_min(0),angle_target(0),angle_max(0){}

Tilt::Output_applicator::Output_applicator(int a):can_address(a){
	assert(can_address>=0 && (unsigned)can_address<Robot_outputs::TALON_SRX_OUTPUTS);
}

Tilt::Input_reader::Input_reader(int a):can_address(a){
	assert(can_address>=0 && (unsigned)can_address<Robot_outputs::TALON_SRX_OUTPUTS);
}

Tilt::Tilt(int can_address):
	output_applicator(can_address),
	input_reader(can_address)
{}

Robot_inputs Tilt::Input_reader::operator()(Robot_inputs all,Tilt::Input in)const{
        auto &t=all.talon_srx[can_address];
        t.fwd_limit_switch=in.top;
        t.rev_limit_switch=in.bottom;
        t.encoder_position=in.ticks;
        t.current=in.current;
        return all;
}

Tilt::Input Tilt::Input_reader::operator()(Robot_inputs all)const{
        auto &t=all.talon_srx[can_address];
        return Tilt::Input{
                t.fwd_limit_switch,
                t.rev_limit_switch,
                t.encoder_position,
                t.current
        };
}

Tilt::Output Tilt::Output_applicator::operator()(Robot_outputs r)const{
	return r.talon_srx[can_address].power_level;
}

Robot_outputs Tilt::Output_applicator::operator()(Robot_outputs r, Tilt::Output out)const{
	r.talon_srx[can_address].power_level=out;
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

CMP_OPS(Tilt::Input,TILT_INPUT)

bool operator<(Tilt::Status_detail a, Tilt::Status_detail b){
	CMP(type())
	CMP(reached_ends)
	CMP(stalled)
	if(a.type()==Tilt::Status_detail::Type::MID){
		return a.get_angle()<b.get_angle();
	}
	return 0;
}

bool operator==(Tilt::Status_detail a,Tilt::Status_detail b){
	if(a.type()!=b.type()) return 0;
	return ((a.type()!=Tilt::Status_detail::Type::MID || a.get_angle()==b.get_angle()) && a.reached_ends==b.reached_ends && a.stalled==b.stalled);
}

bool operator!=(Tilt::Status_detail a,Tilt::Status_detail b){ return !(a==b); }

bool operator==(Tilt::Goal a, Tilt::Goal b){ return (a.mode()==b.mode() && a.angle()==b.angle()); }
bool operator!=(Tilt::Goal a, Tilt::Goal b){ return !(a==b); }

bool operator<(Tilt::Goal a, Tilt::Goal b){
	if(a.mode()==b.mode()) {
		if(a.mode()==Tilt::Goal::Mode::GO_TO_ANGLE) return a.angle()<b.angle();
		return 0;
	}
	return a.mode()<b.mode();
}

bool operator==(Tilt::Output_applicator a,Tilt::Output_applicator b){ return a.can_address==b.can_address; }
bool operator==(Tilt::Input_reader a,Tilt::Input_reader b){ return a.can_address==b.can_address; }
bool operator==(Tilt::Estimator a,Tilt::Estimator b){ return (a.last==b.last && a.top==b.top && a.bottom==b.bottom); }
bool operator!=(Tilt::Estimator a,Tilt::Estimator b){ return !(a==b); }

bool operator==(Tilt a, Tilt b){ return (a.output_applicator==b.output_applicator && a.input_reader==b.input_reader && a.estimator==b.estimator); }
bool operator!=(Tilt a, Tilt b){ return !(a==b); }

std::set<Tilt::Input> examples(Tilt::Input*){ 
	return  {
		Tilt::Input{0,0,0,0},
		Tilt::Input{0,1,0,0},
		Tilt::Input{1,0,0,0},
		Tilt::Input{1,1,0,0}
	};
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

void Tilt::Estimator::update(Time, Tilt::Input, Tilt::Output) {

}

Tilt::Status_detail Tilt::Estimator::get()const {
	return last;
}

#ifdef TILT_TEST
#include "formal.h"

int main(){
	Tilt a(1);
	tester(a);	
}
#endif
