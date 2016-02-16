#include "main.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <math.h>
#include <stdlib.h>
#include "toplevel.h"
#include "../util/util.h"
#include "../input/util.h"
#include <vector>
#include <assert.h>
#include <fstream> 


using namespace std;

ofstream myfile2;

ostream& operator<<(ostream& o,Main::Mode a){
	#define X(NAME) if(a==Main::Mode::NAME) return o<<""#NAME;
	MODES
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Main::Collector_mode a){
	#define X(name) if(a==Main::Collector_mode::name) return o<<"Main::Collector_mode("#name")";
	COLLECTOR_MODES
	#undef X
	assert(0);
}

//todo: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():mode(Mode::TELEOP),autonomous_start(0),collector_mode(Collector_mode::NOTHING){}

vector<Main::NavS> Main::loadnav(){
	vector<NavS> nav;
	NavS navelement;
	navinput start, end;
	vector<pair<int,movedir>> v;
	ofstream myfile;
	myfile.open("/home/lvuser/logs/navlog.txt");
	myfile << "hi" << "\n";
	myfile.flush();
	//assign start information
	start.navpt.x = 5;
	start.navpt.y = 150;
	start.navdir = DOWN;
	
	//assign end information
	end.navpt.x = 5;
	end.navpt.y = 155;
	end.navdir = DOWN;
	
	v=solvemaze(start.navpt,end.navpt,start.navdir,end.navdir);
	myfile << "size: " << v.size() << "\n"; 
	//something to note is that doing a 180 or going back is going to be the same as turning exept that it is going to be for longer so that it can go as far  
	for (unsigned int i=0;i<v.size();i++){
		myfile << "Processing " << i <<  "\n";
		if(v[i].second == MFORWARD){
			navelement.left = -.45;
			navelement.right = -.45;
			navelement.amount = v[i].first;
		}
		else if(v[i].second == MLEFT){
			navelement.left = .45;
			navelement.right = -.45;
			navelement.amount = 1;
		}
		else if(v[i].second == MRIGHT){
			navelement.left= -.45;
			navelement.right= .45;
			navelement.amount= 1;
		}
		else if(v[i].second == MBACK){
			navelement.left= .45;
			navelement.right= -.45;
			navelement.amount= 2;
		}
		else assert(0);
		myfile << "Pushing " << "navelm.left " << navelement.left << " navelm.right " << navelement.right << " amount " << navelement.amount << endl;
		nav.push_back(navelement);

		navelement.left= 0;
		navelement.right= 0;
		navelement.amount= 1;
		nav.push_back(navelement);
		myfile << "Pushing " << "navelm.left " << navelement.left << " navelm.right " << navelement.right << " amount " << navelement.amount << endl;
	}
	myfile.close();
	return nav;
}

double set_drive_speed(double axis,double boost,double slow){
	static const float MAX_SPEED=1;//Change this value to change the max speed the robot will achieve with full boost
	static const float DEFAULT_SPEED=.5;//Change this value to change the default speed
	static const float SLOW_BY=.5;//Change this value to change the percentage of the default speed the slow button slows
	return (pow(axis,3)*((DEFAULT_SPEED+(MAX_SPEED-DEFAULT_SPEED)*boost)-((DEFAULT_SPEED*SLOW_BY)*slow)));
}

template<size_t LEN>
array<double,LEN> floats_to_doubles(array<float,LEN> a){
	array<double,LEN> r;
	for(size_t i=0;i<LEN;i++) r[i]=a[i];
	return r;
}

Toplevel::Goal Main::teleop(
	Robot_inputs const& in,
	Joystick_data const& main_joystick,
	Joystick_data const& gunner_joystick,
	Panel const&  oi_panel,
	Toplevel::Status_detail& //toplevel_status
){
	Toplevel::Goal goals;

	bool spin=fabs(main_joystick.axis[Gamepad_axis::RIGHTX])>.01,boost=main_joystick.axis[Gamepad_axis::LTRIGGER],slow=main_joystick.axis[Gamepad_axis::RTRIGGER];//spin, turbo, and slow buttons	
	
	static const double NUDGE_POWER=.4,NUDGE_CW_POWER=.4,NUDGE_CCW_POWER=-.4;
 
	goals.drive.left=[&]{
		double power=set_drive_speed(main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
		if(spin) power+=set_drive_speed(-main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
		if(!nudges[Nudges::FORWARD].timer.done()) power=-NUDGE_POWER;
		else if(!nudges[Nudges::BACKWARD].timer.done()) power=NUDGE_POWER;
		else if(!nudges[Nudges::CLOCKWISE].timer.done()) power=-NUDGE_CW_POWER;
		else if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) power=-NUDGE_CCW_POWER;
		return power;
	}();
	goals.drive.right=[&]{
		double power=set_drive_speed(main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
		if(spin) power+=set_drive_speed(main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
		if(!nudges[Nudges::FORWARD].timer.done()) power=-NUDGE_POWER;
		else if(!nudges[Nudges::BACKWARD].timer.done()) power=NUDGE_POWER;
		else if(!nudges[Nudges::CLOCKWISE].timer.done()) power=NUDGE_CW_POWER;
		else if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) power=NUDGE_CCW_POWER;
		return power;
	}();

	static const unsigned int nudge_buttons[NUDGES]={Gamepad_button::Y,Gamepad_button::A,Gamepad_button::B,Gamepad_button::X};//Forward, backward, clockwise, counter-clockwise
	for(int i=0;i<Nudges::NUDGES;i++){
		bool start=nudges[i].trigger(boost<.25 && main_joystick.button[nudge_buttons[i]]);
		if(start)nudges[i].timer.set(.1);
		nudges[i].timer.update(in.now,1);
	}
		
	//bool has_ball=(in.digital_io.in[6]==Digital_in::_1);

	/*controller_auto.update(gunner_joystick.button[Gamepad_button::START]);
	if (!oi_panel.in_use || (oi_panel.in_use && oi_panel.collector_auto) || controller_auto.get()) {
		if(main_joystick.button[Gamepad_button::BACK])collector_mode=Collector_mode::NOTHING;
		if(main_joystick.button[Gamepad_button::START]) {
			collector_mode=(toplevel_status.tilt.type() == Tilt::Status_detail::Type::TOP) ? (has_ball ? Collector_mode::REFLECT : Collector_mode::COLLECT) : Collector_mode::STOW;
		}
		
		switch(collector_mode){
			case Collector_mode::COLLECT:
				goals.front=Front::Goal::IN;
				goals.sides=Sides::Goal::IN;
				goals.tilt=Tilt::Goal::down();
			case Collector_mode::STOW:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::up();
				break;
			case Collector_mode::REFLECT:
				goals.front=Front::Goal::OUT;
				goals.sides=Sides::Goal::OUT;
				goals.tilt=Tilt::Goal::down();
				break;
			case Collector_mode::EJECT:
				goals.front=Front::Goal::OUT;
				goals.sides=Sides::Goal::IN;
				goals.tilt=Tilt::Goal::down();
				break;
			case Collector_mode::TERRAIN:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::up();
				break;
			case Collector_mode::LOW_BAR:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::down();
				break;
			case Collector_mode::NOTHING:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::stop();
				break;
			default: assert(0);
		}
	} else */{
		goals.front=[&]{
			if(gunner_joystick.button[Gamepad_button::A]) return Front::Goal::IN;
			else if(gunner_joystick.button[Gamepad_button::Y]) return Front::Goal::OUT;
			else if(oi_panel.in_use) {
				#define X(name) if(oi_panel.front==Panel::Collector::name) return Front::Goal::name;
				X(IN) X(OUT) X(OFF)
				#undef X
				assert(0);
			}
			else return Front::Goal::OFF;
		}();
		goals.sides=[&]{
			if(gunner_joystick.button[Gamepad_button::X]) return Sides::Goal::IN;
			else if(gunner_joystick.button[Gamepad_button::B]) return Sides::Goal::OUT;
			else if(oi_panel.in_use){
				#define X(name) if(oi_panel.sides==Panel::Collector::name) return Sides::Goal::name;
				X(IN) X(OUT) X(OFF)
				#undef X
				assert(0);
			}
			else return Sides::Goal::OFF;
		}();
		goals.tilt=[&]{
			if(gunner_joystick.button[Gamepad_button::LB]) return Tilt::Goal::down();
			else if(gunner_joystick.button[Gamepad_button::RB]) return Tilt::Goal::up();
			else if(gunner_joystick.button[Gamepad_button::BACK]) return Tilt::Goal::stop();
			else if(gunner_joystick.button[Gamepad_button::R_JOY]) return Tilt::Goal::go_to_angle(array<double,3>{18,20,22});
			else if(oi_panel.in_use){
				switch(oi_panel.tilt){
					case Panel::Tilt::UP: return Tilt::Goal::up();
					case Panel::Tilt::DOWN: return Tilt::Goal::down();
					case Panel::Tilt::STOP: return Tilt::Goal::stop();
					default: assert(0);
				}
				if(oi_panel.control_angle){
					array<double,3> angles={oi_panel.angle-2,oi_panel.angle,oi_panel.angle+2};//Assuming tolerances for now
					return Tilt::Goal::go_to_angle(angles);
				}
			}
			return goals.tilt;
		}();
		goals.climb=[&]{
			if(oi_panel.in_use){
				#define X(name) if(oi_panel.climber==Panel::Climber::name) return Climb::Goal::name;
				X(EXTEND) X(STOP) X(RETRACT)
				#undef X
				assert(0);	
			}
			else return Climb::Goal::STOP;
		}();
	}	
	return goals;
}

Main::Mode next_mode(Main::Mode m,bool autonomous,bool autonomous_start,Toplevel::Status_detail /*status*/,Time since_switch, Panel /*oi_panel*/,unsigned int navindex,std::vector<Main::NavS> NavV){
	
	switch(m){

		case Main::Mode::TELEOP:
			if(autonomous_start){
				return Main::Mode::AUTO_NAV;//just for testing purposes
				/*if (oi_panel.in_use) {
					switch(oi_panel.auto_mode){ 
						case Panel::Auto_mode::CAN_GRAB:
							return Main::Mode::AUTO_GRAB;
						case Panel::Auto_mode::MOVE:
							return Main::Mode::AUTO_MOVE;
						case Panel::Auto_mode::DO_NOTHING:
							return Main::Mode::TELEOP;
						default: assert(0);
					}
				} else {
					return Main::Mode::AUTO_GRAB;
				}*/
				return Main::Mode::TELEOP;
			}
			return m;
		case Main::Mode::AUTO_MOVE:
			//encoders? going to use time for now
			if(!autonomous || since_switch>1) return Main::Mode::TELEOP;
			return m;
		case Main::Mode::AUTO_NAV:
			return Main::Mode::AUTO_NAV_RUN;
		case Main::Mode::AUTO_NAV_RUN:
			if(since_switch>NavV[navindex].amount) {
				navindex++;
				myfile2 << "navindex:" << navindex << endl;
				myfile2.flush();
			} 
			if(navindex==NavV.size()) {
				myfile2 << "done" << endl;
				myfile2.flush();
				return Main::Mode::TELEOP;
			}
			return Main::Mode::AUTO_NAV_RUN;
		default: assert(0);
	}
}

Robot_outputs Main::operator()(Robot_inputs in,ostream&){
	perf.update(in.now);
	Joystick_data main_joystick=in.joystick[0];
	Joystick_data gunner_joystick=in.joystick[1];
	Panel oi_panel=interpret(in.joystick[2]);

	/*if(oi_panel.in_use && (!in.robot_mode.enabled || in.robot_mode.autonomous)){
		Panel empty;
		oi_panel=empty;
	}*/

	force.update(
		main_joystick.button[Gamepad_button::A],
		main_joystick.button[Gamepad_button::LB],
		main_joystick.button[Gamepad_button::RB],
		main_joystick.button[Gamepad_button::BACK],
		main_joystick.button[Gamepad_button::B],
		main_joystick.button[Gamepad_button::X]
	);
	
	Toplevel::Status_detail toplevel_status=toplevel.estimator.get();
	
	cout<<"panel: "<<oi_panel<<"\n";	
		
	bool autonomous_start_now=autonomous_start(in.robot_mode.autonomous && in.robot_mode.enabled);
	since_auto_start.update(in.now,autonomous_start_now);
		
	Toplevel::Goal goals;
	//Drivebase::Status_detail status_detail = drivebase.estimator.get();
	
	switch(mode){
		case Mode::TELEOP:
			goals=teleop(in,main_joystick,gunner_joystick,oi_panel,toplevel_status);
			break;
		case Mode::AUTO_MOVE:
			goals.drive.left=.45;
			goals.drive.right=.45;
			break;
		case Mode::AUTO_NAV:
			myfile2.open("/home/lvuser/logs/navlog2.txt");
			NavV = loadnav();
			navindex = 0;
			myfile2 << "Nav loaded:" << NavV.size() << endl;
			myfile2.flush();
			break;
		case Mode::AUTO_NAV_RUN:
			goals.drive.left=NavV[navindex].left;
			goals.drive.right=NavV[navindex].right;
			break;
		default: assert(0);
	}
	auto next=next_mode(mode,in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),oi_panel,navindex,NavV);
	since_switch.update(in.now,mode!=next);
	mode=next;

	Toplevel::Output r_out=control(toplevel_status,goals); 
	auto r=toplevel.output_applicator(Robot_outputs{},r_out);
	r=force(r);
	auto input=toplevel.input_reader(in);

	toplevel.estimator.update(
		in.now,
		input,
		/*Toplevel::Input{
			{can_input,tote_input},
			Kicker::Input{},
			drive_in,
			Pump::Input::NOT_FULL,
			Can_grabber::Input{1}, //todo: make this actually ready from a digital io
			Tote_sensors::Input{0,0,0}
		},*/
		toplevel.output_applicator(r)
	);
	return r;
}

bool operator==(Main a,Main b){
	return a.force==b.force && 
		a.perf==b.perf && 
		a.toplevel==b.toplevel && 
		a.since_switch==b.since_switch && 
		a.since_auto_start==b.since_auto_start &&
		a.autonomous_start==b.autonomous_start;
}

bool operator!=(Main a,Main b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Main m){
	o<<"Main(";
	o<<m.mode;
	o<<m.force;
	o<<m.perf;
	o<<m.toplevel;
	o<<m.since_switch;
	return o<<")";
}

bool approx_equal(Main a,Main b){
	if(a.force!=b.force) return 0;
	if(a.toplevel!=b.toplevel) return 0;
	return 1;
}

#ifdef MAIN_TEST
#include<fstream>
#include "monitor.h"

template<typename T>
vector<T> uniq(vector<T> v){
	vector<T> r;
	for(auto a:v){
		if(!(r.size()&&r[r.size()-1]==a)){
			r|=a;
		}
	}
	return r;
}

int main(){}

#endif
