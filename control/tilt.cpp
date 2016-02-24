#include "tilt.h"
#include <stdlib.h>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 

enum Positions{TOP,LEVEL,LOW,BOTTOM,POSITIONS};
std::array<float,Positions::POSITIONS> positions={1.3,2.3,2.7,3.6};//in volts
static const std::array<std::string,Positions::POSITIONS> POSITION_NAMES={"TOP","LEVEL","LOW","BOTTOM"};	
static const std::string POSITIONS_PATH="/home/lvuser/";//path for use on the robot
static const std::string POSITIONS_FILE=[&]{//The name of the file were it stores the preset positions
	std::string s;
	#ifndef TILT_TEST
	s=POSITIONS_PATH;
	#endif
	return s.append("tilt_positions.txt");
}();

#define POWER 1//negative goes up, positive goes down

#define VOLTS_PER_DEGREE .03// (in volts/degree) //Assumed for now

#define ANGLE_TOLERANCE 5//in degrees

#define PI 3.14159265//It's pi. What more do you want?

#define TILT_PDB_LOC 8
#define TILT_POT_LOC 0
#define TILT_LIM_LOC 9
#define TILT_ADDRESS 4

#define nyi { std::cout<<"\nnyi "<<__LINE__<<"\n"; exit(44); }

float volts_to_degrees(float f){
	return f/VOLTS_PER_DEGREE;
}

float degrees_to_volts(float f){
	return f*VOLTS_PER_DEGREE;
}

double degree_change_to_power(double start, double end) { //start and end are in degrees
	const double SCALE_DOWN=.01;
	double error = end-start;
	return error * SCALE_DOWN;
}

double power_to_keep_up(double angle) {
	return -(sin(angle*PI/180) * (81.0 / 1750));
}

Tilt::Status_detail::Status_detail(): 
	reached_ends(std::make_pair(0,0)),
	stalled(0),
	type_(Tilt::Status_detail::Type::MID),
	angle(0),
	pot_value_(0)
{}

Tilt::Status::Status(Tilt::Status::Type type,double angle){
	this->type=type;
	this->angle=angle;
}

Tilt::Goal::Goal():mode_(Tilt::Goal::Mode::STOP),angle_min(0),angle_target(0),angle_max(0),learn_bottom(0){}

Robot_inputs Tilt::Input_reader::operator()(Robot_inputs r,Tilt::Input in)const{
	r.current[TILT_PDB_LOC]=in.current;
	r.analog[TILT_POT_LOC]=in.pot_value;
	r.digital_io.in[TILT_LIM_LOC]=in.top ? Digital_in::_0 : Digital_in::_1;
	return r;
}

Tilt::Input Tilt::Input_reader::operator()(Robot_inputs r)const{
	return {r.analog[TILT_POT_LOC],r.current[TILT_PDB_LOC],r.digital_io.in[TILT_LIM_LOC]==Digital_in::_0};
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

Tilt::Goal Tilt::Goal::low(){
	return Tilt::Goal::go_to_angle(make_tolerances(volts_to_degrees(positions[Positions::LOW])));
}

Tilt::Goal Tilt::Goal::level(){
	return Tilt::Goal::go_to_angle(make_tolerances(volts_to_degrees(positions[Positions::LEVEL])));
}

Tilt::Status_detail::Type Tilt::Status_detail::type()const{
	return type_;
}

double Tilt::Status_detail::get_angle()const{
	return angle;
}

float Tilt::Status_detail::pot_value()const{
	return pot_value_;
}

Tilt::Status_detail Tilt::Status_detail::top(){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::TOP;
	return a;
}

Tilt::Status_detail Tilt::Status_detail::mid(double angle,double pot_in){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::MID;
	a.angle=angle;
	a.pot_value_=pot_in;
	return a;
}

Tilt::Status_detail Tilt::Status_detail::bottom(double angle=volts_to_degrees(positions[Positions::BOTTOM]), double pot_in=positions[Positions::BOTTOM]){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::BOTTOM;
	a.angle=angle;
	a.pot_value_=pot_in;
	return a;
}

Tilt::Status_detail Tilt::Status_detail::error(){
	Tilt::Status_detail a;
	a.type_=Tilt::Status_detail::Type::ERRORS;
	return a;
}

std::ostream& operator<<(std::ostream& o, Tilt::Status_detail::Type a){
	#define X(name) if(a==Tilt::Status_detail::Type::name) return o<<""#name;
	TILT_STATUS_DETAIL_TYPES
	#undef X
	nyi
}

std::ostream& operator<<(std::ostream& o, Tilt::Status a){
	return o<<"Tilt::Status( type:"<<a.type<<" angle:"<<a.angle<<")";
}

std::ostream& operator<<(std::ostream& o, Tilt::Goal::Mode a){
	#define X(name) if(a==Tilt::Goal::Mode::name) return o<<"Tilt::Goal("#name")";
	TILT_GOAL_MODES
	#undef X
	nyi
}

std::ostream& operator<<(std::ostream& o, Tilt::Status_detail a){ 
	o<<"Tilt::Status_detail(";
	o<<" stalled:"<<a.stalled;
	o<<" reached_ends:"<<a.reached_ends;
	o<<" type:"<<a.type();
	if(a.type()==Tilt::Status_detail::Type::MID)o<<"("<<a.get_angle()<<" "<<a.pot_value()<<")";
	return o<<")";
}

std::ostream& operator<<(std::ostream& o, Tilt::Goal a){ 
	o<<"Tilt::Goal(";
	o<<" mode:"<<a.mode();
	if(a.mode()==Tilt::Goal::Mode::GO_TO_ANGLE)o<<"("<<a.angle()<<")";
	o<<" learn_bottom:"<<a.learn_bottom;
	return o<<")";
}

std::ostream& operator<<(std::ostream& o, Tilt::Output_applicator){ return o<<"Tilt::Output_applicator()";} 
std::ostream& operator<<(std::ostream& o, Tilt::Input_reader){ return o<<"Tilt::Input_reader()";}
std::ostream& operator<<(std::ostream& o, Tilt::Estimator a){ return o<<"Tilt::Estimator( last:"<<a.get()<<" stall_timer:"<<a.stall_timer<<" timer_start_angle:"<<a.timer_start_angle<<")";} 
 
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
bool operator<(Tilt::Input const& a,Tilt::Input const& b){
	CMP(pot_value)
	CMP(current)
	return !a.top && b.top;
}
std::ostream& operator<<(std::ostream& o,Tilt::Input const& a){ return o<<"Tilt::Input( pot_value:"<<a.pot_value<<" current:"<<a.current<<" top:"<<a.top<<")"; }

bool operator<(Tilt::Status a, Tilt::Status b){
	CMP(type)
	if(a.type==Tilt::Status::Type::MID)return a.angle<b.angle;
	return false;
}
bool operator==(Tilt::Status a,Tilt::Status b){
	return a.type==b.type && (a.type==Tilt::Status::Type::MID ? a.angle==b.angle : true);
}
bool operator!=(Tilt::Status a,Tilt::Status b){ return !(a==b); }

bool operator<(Tilt::Status_detail a, Tilt::Status_detail b){
	CMP(type())
	if(a.type()==Tilt::Status_detail::Type::MID){
		CMP(pot_value())
		CMP(get_angle())
	}
	CMP(reached_ends)
	return !a.stalled && b.stalled;
}
bool operator==(Tilt::Status_detail a,Tilt::Status_detail b){
	if(a.type()!=b.type()) return 0;
	return ((a.type()==Tilt::Status_detail::Type::MID ? (a.get_angle()==b.get_angle() && a.pot_value()==b.pot_value()) : true) && a.reached_ends==b.reached_ends && a.stalled==b.stalled);
}
bool operator!=(Tilt::Status_detail a,Tilt::Status_detail b){ return !(a==b); }

bool operator==(Tilt::Goal a, Tilt::Goal b){ 
	return a.mode()==b.mode() &&  a.learn_bottom==b.learn_bottom && (a.mode()==Tilt::Goal::Mode::GO_TO_ANGLE ? a.angle()==b.angle() : true );
}
bool operator!=(Tilt::Goal a, Tilt::Goal b){ return !(a==b); }
bool operator<(Tilt::Goal a, Tilt::Goal b){
	if(a.mode()<b.mode()) return true;
	if(b.mode()<a.mode()) return false;
	if(a.mode()==Tilt::Goal::Mode::GO_TO_ANGLE) return a.angle()<b.angle();
	return a.learn_bottom && !b.learn_bottom;
}

bool operator==(Tilt::Output_applicator,Tilt::Output_applicator){ return true; }

bool operator==(Tilt::Input_reader,Tilt::Input_reader){ return true; }

bool operator==(Tilt::Estimator a,Tilt::Estimator b){ return a.last==b.last; }
bool operator!=(Tilt::Estimator a,Tilt::Estimator b){ return !(a==b); }

bool operator==(Tilt a, Tilt b){ return (a.output_applicator==b.output_applicator && a.input_reader==b.input_reader && a.estimator==b.estimator); }
bool operator!=(Tilt a, Tilt b){ return !(a==b); }

std::set<Tilt::Input> examples(Tilt::Input*){ 
	std::set<Tilt::Input> s;
	for(unsigned int i=0; i<Positions::POSITIONS; i++){
		s.insert({positions[i],0,i==Positions::TOP});
	}
	return s;
}

std::set<Tilt::Goal> examples(Tilt::Goal*){
	return {
		Tilt::Goal::down(),
		Tilt::Goal::stop(),
		Tilt::Goal::low(),
		Tilt::Goal::level(),
		Tilt::Goal::go_to_angle(make_tolerances(ANGLE_TOLERANCE)),
		Tilt::Goal::up()
	};
}

std::set<Tilt::Status_detail> examples(Tilt::Status_detail*){
	return {
		Tilt::Status_detail::top(),
		Tilt::Status_detail::mid(0,0),
		Tilt::Status_detail::bottom(),
		Tilt::Status_detail::error()
	};
}

std::set<Tilt::Status> examples(Tilt::Status*){
	std::set<Tilt::Status> s;
	#define X(name) s.insert({Tilt::Status::Type::name,0});
	TILT_STATUS_DETAIL_TYPES
	#undef X
	return s;
}

std::set<Tilt::Output> examples(Tilt::Output*){ 
	return {
		-POWER,
		0,
		degree_change_to_power(0, volts_to_degrees(positions[Positions::LEVEL])),
		degree_change_to_power(0, volts_to_degrees(positions[Positions::LOW])),
		POWER
	};
}

Tilt::Output control(Tilt::Status_detail status, Tilt::Goal goal){
	const double SLOW_FULL_DESCENT=.2, SLOW=.5*POWER;
	if(goal.learn_bottom) return POWER*SLOW_FULL_DESCENT; 
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
		case Tilt::Goal::Mode::GO_TO_ANGLE:
			switch (status.type()) {
				case Tilt::Status_detail::Type::MID:
					if(status.get_angle()>=goal.angle()[0] && status.get_angle()<=goal.angle()[2])return power_to_keep_up(goal.angle()[1]);
					return (degree_change_to_power(status.get_angle(),goal.angle()[1]));
				case Tilt::Status_detail::Type::TOP:
					return SLOW;
				case Tilt::Status_detail::Type::BOTTOM:
					return -SLOW;
				case Tilt::Status_detail::Type::ERRORS:
					return 0.0;
				default:
					assert(0);
			}
		case Tilt::Goal::Mode::STOP: return power_to_keep_up(status.get_angle());
		default: assert(0);
	}
}

Tilt::Status status(Tilt::Status_detail a){
	return {a.type(),a.get_angle()};
}

bool ready(Tilt::Status status, Tilt::Goal goal){
	switch(goal.mode()){
		case Tilt::Goal::Mode::UP: return status.type==Tilt::Status::Type::TOP;
		case Tilt::Goal::Mode::DOWN: return (status.type==Tilt::Status::Type::BOTTOM && !goal.learn_bottom);
		case Tilt::Goal::Mode::GO_TO_ANGLE: return (status.angle>=goal.angle()[0] && status.angle<=goal.angle()[2]);
		case Tilt::Goal::Mode::STOP: return 1;
		default: assert(0);
	}
}

Tilt::Status_detail Tilt::Estimator::get()const {
	return last;
}

void Tilt::Estimator::update(Time time, Tilt::Input in, Tilt::Output) {
	//update_positions();
	if(in.top) tilt_learn(in.pot_value,POSITION_NAMES[Positions::TOP]);
	const float ALLOWED_TOLERANCE=degrees_to_volts(15);//15 degrees is a good tolerane for the bottom
	bool at_top=in.pot_value<=positions[Positions::TOP]+ALLOWED_TOLERANCE, at_bottom=in.pot_value>=positions[Positions::BOTTOM]-ALLOWED_TOLERANCE;
	float angle=volts_to_degrees(in.pot_value-positions[Positions::TOP]);
	stall_timer.update(time,true);
	if(stall_timer.done()) last.stalled=true;
	if(in.current<10 || fabs(angle-timer_start_angle)<1){//Assumed current for now
		last.stalled=0;
		stall_timer.set(1);
		timer_start_angle=angle;
	}
	if(in.top && at_top){
		if(at_bottom)last=Tilt::Status_detail::error();
		else last=Tilt::Status_detail::top();
	} else{
		if(at_bottom) last=Tilt::Status_detail::bottom(angle,in.pot_value);
		else last=Tilt::Status_detail::mid(angle,in.pot_value);
		if(in.pot_value>positions[Positions::BOTTOM])tilt_learn(in.pot_value,POSITION_NAMES[Positions::BOTTOM]);
	}
}

std::array<double,3> make_tolerances(double d){
	return {d-ANGLE_TOLERANCE,d,d+ANGLE_TOLERANCE};
}

void populate(){
	std::ifstream test(POSITIONS_FILE);
	assert(test.peek()==std::ifstream::traits_type::eof());//file is empty
	test.close();
	std::ofstream file(POSITIONS_FILE);
	for(unsigned int i=0; i<Positions::POSITIONS; i++)file<<POSITION_NAMES[i]<<":"<<positions[i]<<(i+1<Positions::POSITIONS ? "\n" : "");
	file.close();
}

void update_positions(){
	std::ifstream file(POSITIONS_FILE);
	if(file.peek()==std::ifstream::traits_type::eof()){
		file.close();
		populate();
		file.open(POSITIONS_FILE);
	}
	for(unsigned int i=0; i<Positions::POSITIONS; i++){
		bool next=false;
		std::string mode=POSITION_NAMES[i];
		while(!file.eof()){ 
			std::string edit,line; 
			std::getline(file,line); 
			for(char c:line){ 
				if(c==':' && edit==mode){ 
					std::istringstream in(line.substr(edit.size()+1));
					float value;
					in>>value;
					positions[i]=value;
					next=true;
					break; 
				} 
				edit+=c; 
			} 
			if(next)break;
		} 
	}
	file.close();
}

void tilt_learn(float pot_in,std::string const& mode){
	assert(mode==POSITION_NAMES[Positions::TOP] || mode==POSITION_NAMES[Positions::LOW] || mode==POSITION_NAMES[Positions::LEVEL] || mode==POSITION_NAMES[Positions::BOTTOM]);
	std::cout<<"\nTrying to learn "<<mode<<" at "<<pot_in<<"\n";
	std::vector<std::string> go_out;
	{
		std::ifstream file(POSITIONS_FILE);
		if(file.peek()==std::ifstream::traits_type::eof()){
			file.close();
			populate();
			file.open(POSITIONS_FILE);
		}
		while(!file.eof()){ 
			std::string edit,line; 
			std::getline(file,line); 
			if(line.size()==0)continue;
			for(char c:line){ 
				if(c==':' && edit==mode){ 
					std::ostringstream out;
					out<<pot_in; 
					edit+=":"+out.str(); 
					break;  
				} 
				edit+=c; 
			} 
			go_out.push_back(edit); 
		} 
		file.close();
	}
	std::ofstream file(POSITIONS_FILE);
	for(unsigned int i=0; i<go_out.size(); i++)file<<go_out[i]<<(i+1<go_out.size() ? "\n" : "");
	file.close();
	update_positions();
}

#ifdef TILT_TEST
#include "formal.h"

int main(){
	update_positions();
	Tilt a;
	Tester_mode t;
	t.check_outputs_exhaustive = 0;
	tester(a, t);
}

#endif
