#include "tilt.h"
#include <stdlib.h>

#define nyi { std::cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }

std::ostream& operator<<(std::ostream& o, Tilt a){ return o<<"Tilt()"; }
std::ostream& operator<<(std::ostream& o, Tilt::Status_detail::Type a){
	#define X(name) if(a==Tilt::Status_detail::Type::name) return o<<""#name;
	X(ERRORS) X(TOP) X(BOTTOM) X(MID)
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
std::ostream& operator<<(std::ostream& o, Tilt::Goal a){ return o<<"Goal()"; }

Tilt::Status_detail::Status_detail(): 
	reached_ends(std::make_pair(0,0)),
	stalled(0)
{}

Tilt::Output_applicator::Output_applicator(int a):can_address(a){
	assert(can_address>=0 && (unsigned)can_address<Robot_outputs::TALON_SRX_OUTPUTS);
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

Robot_outputs Tilt::Output_applicator::operator()(Robot_outputs r, Tilt::Output out)const{
	r.talon_srx[can_address].power_level=out;
	return r;
}

Tilt::Output Tilt::Output_applicator::operator()(Robot_outputs r)const{
	return r.talon_srx[can_address].power_level;
}

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
std::set<Tilt::Output> examples(Tilt::Output*){ return std::set<Tilt::Output>{Tilt::Output{}}; }

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
			nyi
		case Tilt::Goal::Mode::STOP: return 0;
		default: assert(0);
	}
}

Tilt::Status status(Tilt::Status_detail a){
	return a;
}

bool ready(Tilt::Status, Tilt::Goal){
	return 1;
}

#ifdef TILT_TEST
#include "formal.h"

int main(){
	Tilt a(1);
	tester(a);	
}
#endif
