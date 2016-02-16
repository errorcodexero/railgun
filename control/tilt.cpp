#include "tilt.h"
#include <stdlib.h>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 

float TILT_POT_TOP=0.00;
float TILT_POT_LEVEL=1.00;
float TILT_POT_BOT=2.00;
#define TILT_PDB_LOC 8
#define TILT_POT_LOC 0
#define TILT_LIM_LOC 9
#define TILT_ADDRESS 4
#define TILT_POSITIONS "tilt_positions.txt"

#define nyi { std::cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }

Tilt::Status_detail::Status_detail(): 
	reached_ends(std::make_pair(0,0)),
	stalled(0),
	type_(Tilt::Status_detail::Type::MID),
	pot_value(TILT_POT_TOP)
{}

Tilt::Goal::Goal():mode_(Tilt::Goal::Mode::STOP){}

Robot_inputs Tilt::Input_reader::operator()(Robot_inputs r,Tilt::Input in)const{
	r.current[TILT_PDB_LOC]=in.current;
	r.analog[TILT_POT_LOC]=in.pot_value;
	r.digital_io.in[TILT_LIM_LOC]=in.top ? Digital_in::_1 : Digital_in::_0;
	return r;
}

Tilt::Input Tilt::Input_reader::operator()(Robot_inputs r)const{
	return {r.analog[TILT_POT_LOC],r.current[TILT_PDB_LOC],r.digital_io.in[TILT_LIM_LOC]==Digital_in::_1};
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
	timer_start_pot_value(0)
{}

Tilt::Goal Tilt::Goal::up(){
	Tilt::Goal a;
	a.mode_=Tilt::Goal::Mode::UP;
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

Tilt::Goal Tilt::Goal::low(){
	Tilt::Goal a;
	a.mode_=Tilt::Goal::Mode::LOW;
	return a;
}

Tilt::Goal Tilt::Goal::level(){
	Tilt::Goal a;
	a.mode_=Tilt::Goal::Mode::LEVEL;
	return a;
}

Tilt::Status_detail::Type Tilt::Status_detail::type()const{
	return type_;
}

double Tilt::Status_detail::get_pot_value()const{
	return pot_value;
}

Tilt::Status_detail Tilt::Status_detail::top(){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::TOP;
	return a;
}

Tilt::Status_detail Tilt::Status_detail::mid(double d){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::MID;
	a.pot_value=d;
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
	TILT_GOAL_MODES
	#undef X
	nyi
}

std::ostream& operator<<(std::ostream& o, Tilt::Status_detail a){ 
	o<<"Tilt::Status_detail(";
	o<<" stalled:"<<a.stalled;
	o<<" reached_ends:"<<a.reached_ends;
	o<<" type:"<<a.type();
	if(a.type()==Tilt::Status_detail::Type::MID){
		o<<" "<<a.get_pot_value();
	}
	return o<<")";
}

std::ostream& operator<<(std::ostream& o, Tilt::Goal a){ 
	o<<"Tilt::Goal(";
	o<<" mode:"<<a.mode();
	return o<<")";
}

std::ostream& operator<<(std::ostream& o, Tilt::Output_applicator){ return o<<"Tilt::Output_applicator()";} 
std::ostream& operator<<(std::ostream& o, Tilt::Input_reader){ return o<<"Tilt::Input_reader()";}
std::ostream& operator<<(std::ostream& o, Tilt::Estimator a){ return o<<"Tilt::Estimator( last:"<<a.get()<<" stall_timer:"<<a.stall_timer<<" timer_start_pot_value:"<<a.timer_start_pot_value<<")";} 
 
std::ostream& operator<<(std::ostream& o, Tilt a){
	o<<"Tilt(";
	o<<" "<<a.output_applicator;
	o<<" "<<a.input_reader;
	o<<" "<<a.estimator;
	return o<<")";
}

#define CMP(name) if(a.name<b.name) return 1; if(b.name<a.name) return 0;

bool operator==(Tilt::Input const& a,Tilt::Input const& b){
	return a.pot_value==b.pot_value && a.current==b.current && a.top==b.top;	
}

bool operator!=(Tilt::Input const& a,Tilt::Input const& b){ return !(a==b); }

bool operator<(Tilt::Input const& a,Tilt::Input const& b){ return a.pot_value<b.pot_value; }

std::ostream& operator<<(std::ostream& o,Tilt::Input const& a){ return o<<"Tilt::Input( pot_value:"<<a.pot_value<<" current:"<<a.current<<" top:"<<a.top<<")"; }

bool operator<(Tilt::Status_detail a, Tilt::Status_detail b){
	CMP(type())
	if(a.type()==Tilt::Status_detail::Type::MID)return a.get_pot_value()<b.get_pot_value();
	CMP(reached_ends)
	return !a.stalled && b.stalled;
}

bool operator==(Tilt::Status_detail a,Tilt::Status_detail b){
	if(a.type()!=b.type()) return 0;
	return ((a.type()!=Tilt::Status_detail::Type::MID || a.get_pot_value()==b.get_pot_value()) && a.reached_ends==b.reached_ends && a.stalled==b.stalled);
}

bool operator!=(Tilt::Status_detail a,Tilt::Status_detail b){ return !(a==b); }

bool operator==(Tilt::Goal a, Tilt::Goal b){ return a.mode()==b.mode(); }
bool operator!=(Tilt::Goal a, Tilt::Goal b){ return !(a==b); }

bool operator<(Tilt::Goal a, Tilt::Goal b){
	return a.mode()<b.mode();
}

bool operator==(Tilt::Output_applicator,Tilt::Output_applicator){ return true; }
bool operator==(Tilt::Input_reader,Tilt::Input_reader){ return true; }
bool operator==(Tilt::Estimator a,Tilt::Estimator b){ return a.last==b.last; }
bool operator!=(Tilt::Estimator a,Tilt::Estimator b){ return !(a==b); }

bool operator==(Tilt a, Tilt b){ return (a.output_applicator==b.output_applicator && a.input_reader==b.input_reader && a.estimator==b.estimator); }
bool operator!=(Tilt a, Tilt b){ return !(a==b); }

std::set<Tilt::Input> examples(Tilt::Input*){ 
	return {{0,0,0},{.5,0,0},{1,0,0},{1.5,0,0},{2,0,0},{0,0,1},{.5,0,1},{1,0,1},{1.5,0,1},{2,0,1}};
}
std::set<Tilt::Goal> examples(Tilt::Goal*){
	return {
		Tilt::Goal::down(),
		Tilt::Goal::stop(),
		Tilt::Goal::low(),
		Tilt::Goal::level(),
		Tilt::Goal::up()
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
	const double POWER=1;//negative goes up, positive goes down
	switch(goal.mode()){
		case Tilt::Goal::Mode::UP:
			switch(status.type()){
				case Tilt::Status_detail::Type::TOP:
				case Tilt::Status_detail::Type::ERRORS:
					return 0;
				case Tilt::Status_detail::Type::BOTTOM:
				case Tilt::Status_detail::Type::MID:
					return -POWER;
				default: assert(0);
			}
		case Tilt::Goal::Mode::DOWN:
			switch(status.type()){
				case Tilt::Status_detail::Type::BOTTOM:
				case Tilt::Status_detail::Type::ERRORS:
					return 0;
				case Tilt::Status_detail::Type::TOP:
				case Tilt::Status_detail::Type::MID:
					return POWER;
				default: assert(0);
			}
		case Tilt::Goal::Mode::LOW:
			switch(status.type()){
				case Tilt::Status_detail::Type::BOTTOM:
				case Tilt::Status_detail::Type::ERRORS:
				case Tilt::Status_detail::Type::TOP:
				case Tilt::Status_detail::Type::MID:
					return 0;//todo change this
				default: assert(0);
			}
		case Tilt::Goal::Mode::LEVEL:
			switch(status.type()){
				case Tilt::Status_detail::Type::BOTTOM:
				case Tilt::Status_detail::Type::ERRORS:
				case Tilt::Status_detail::Type::TOP:
				case Tilt::Status_detail::Type::MID:
					return 0;//todo change this
				default: assert(0);
			}
		/*case Tilt::Goal::Mode::GO_TO_ANGLE:
			{
				const double SLOW=(POWER/5);
				switch (status.type()) {
					case Tilt::Status_detail::Type::MID:
						{
							if(status.get_angle()>=goal.angle()[0] && status.get_angle()<=goal.angle()[2])return 0.0;
							std::cout<<status.get_angle()<<std::endl;
							double corrected_power=-(goal.angle()[1]-status.get_angle())*SLOW;
							if(corrected_power>POWER)return POWER;
							if(corrected_power<-POWER)return -POWER;
							return corrected_power;
						}
					case Tilt::Status_detail::Type::TOP:
						return POWER;
					case Tilt::Status_detail::Type::BOTTOM:
						return -POWER;
					case Tilt::Status_detail::Type::ERRORS:
						return 0.0;
					default:
						assert(0);
				}
			}*/
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
		case Tilt::Goal::Mode::DOWN: return status.type()==Tilt::Status_detail::Type::BOTTOM;
		case Tilt::Goal::Mode::LOW: return 0;
		case Tilt::Goal::Mode::LEVEL: return 0;
		case Tilt::Goal::Mode::STOP: return 1;
		default: nyi
	}
}

template<typename T>
bool in_range(T a, T b, T c){//returns if a is in a range of +/- c from b
	return a>(b-c) && a<(b+c);
}

void Tilt::Estimator::update(Time time, Tilt::Input in, Tilt::Output) {
	if(in.top)TILT_POT_TOP=in.pot_value;
	float pot_value=in.pot_value-TILT_POT_TOP;
	stall_timer.update(time,true);
	if(stall_timer.done()) last.stalled=true;
	if(in.current<10 || fabs(pot_value-timer_start_pot_value)<1){//Assumed current for now
		last.stalled=0;
		stall_timer.set(1);
		timer_start_pot_value=pot_value;
	}
	const float ALLOWED_TOLERANCE=.05;
	if(in.pot_value<=TILT_POT_TOP+ALLOWED_TOLERANCE){
		if(in.pot_value>=TILT_POT_BOT-ALLOWED_TOLERANCE){
			last=Tilt::Status_detail::error();
		} else{
			last=Tilt::Status_detail::top();                       
		}
	} else{
		if(in.pot_value>=TILT_POT_BOT-ALLOWED_TOLERANCE){
			last=Tilt::Status_detail::bottom();
		} else{
			last=Tilt::Status_detail::mid(pot_value);
		}
	}
}

Tilt::Status_detail Tilt::Estimator::get()const {
	return last;
}



void learn(float pot_in,Tilt::Goal::Mode a){
	std::ifstream file(TILT_POSITIONS);
	std::ostringstream out;
	std::vector<std::string> go_out;
	std::string line,mode;
	#define X(name) if(a==Tilt::Goal::Mode::name) mode=""#name;
	X(UP) X(DOWN) X(STOP) X(LOW) X(LEVEL)
	#undef X
	#define X(name) \
		while(!file.eof()){ \
			std::string edit; \
			std::getline(file,line); \
			for(char c:line){ \
				if(c==':'){ \
					if(edit==mode){ \
						out<<pot_in; \
						edit+=":"+out.str(); \
						break; \
					} \
				} \
				edit+=c; \
			} \
			go_out.push_back(edit); \
		} 
	X(UP) X(DOWN) X(STOP) X(LOW) X(LEVEL)
	#undef X
	file.close();
	std::ofstream file2(TILT_POSITIONS);
	for(unsigned int i=0; i<go_out.size(); i++){
		file2<<go_out[i]<<"\r\n";
	}
	file2.close();
}

#ifdef TILT_TEST
#include "formal.h"

int main(){
	Tilt a;
	tester(a);
	learn(2.67,Tilt::Goal::Mode::LOW);	
}

#endif
