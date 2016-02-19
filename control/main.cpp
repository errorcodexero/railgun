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

//TODO: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():mode(Mode::TELEOP),autonomous_start(0),collector_mode(Collector_mode::NOTHING){}

vector<Main::NavS> Main::loadnav(){
	float amount = 0;
	vector<NavS> nav;
	NavS navelement;
	navinput start, end;
	vector<pair<int,movedir>> v;
	ofstream myfile("/home/lvuser/logs/navlog.txt");
	//oiload oinav;

	myfile << "hi" << "\n";
	myfile.flush();
	//assign start information
	start.navpt.x = 5;
	start.navpt.y = 35;
	start.navdir = RIGHT;
	
	//assign end information
	end.navpt.x = 5;
	end.navpt.y = 125;
	end.navdir = DOWN;
	
	v=solvemaze(start.navpt,end.navpt,start.navdir,end.navdir);
	myfile << "size: " << v.size() << "\n"; 
	//something to note is that doing a 180 or going back is going to be the same as turning exept that it is going to be for longer so that it can go as far  

	for (unsigned int i=0;i<v.size();i++){
		myfile << "Processing " << i <<  "\n";
		if(v[i].second == MFORWARD){
			navelement.left = -.45;
			navelement.right = -.45;
			amount += v[i].first / 29.0;
		}
		else if(v[i].second == MLEFT){
			navelement.left = .45;
			navelement.right = -.45;
			amount += .65;
		}
		else if(v[i].second == MRIGHT){
			navelement.left= -.45;
			navelement.right= .45;
			amount += .65;
		}
		else if(v[i].second == MBACK){
			navelement.left= .45;
			navelement.right= -.45;
			amount+= 1.1;
		}
		else assert(0);
		navelement.amount = amount;

		myfile << "Pushing " << "navelm.left " << navelement.left << " navelm.right " << navelement.right << " amount " << navelement.amount << endl;
		nav.push_back(navelement);

		navelement.left= 0;
		navelement.right= 0;
		amount += 1;
		navelement.amount = amount;
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
	Toplevel::Status_detail& toplevel_status
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

	goals.climb_release=[&]{
		if(main_joystick.axis[Gamepad_axis::LTRIGGER]>.5)return Climb_release::Goal::IN;
		if(main_joystick.axis[Gamepad_axis::RTRIGGER]>.5)return Climb_release::Goal::OUT;
		return Climb_release::Goal::STOP;
	}();

	static const unsigned int nudge_buttons[NUDGES]={Gamepad_button::Y,Gamepad_button::A,Gamepad_button::B,Gamepad_button::X};//Forward, backward, clockwise, counter-clockwise
	for(int i=0;i<Nudges::NUDGES;i++){
		bool start=nudges[i].trigger(boost<.25 && main_joystick.button[nudge_buttons[i]]);
		if(start)nudges[i].timer.set(.1);
		nudges[i].timer.update(in.now,1);
	}
		
	/*bool has_ball=(in.digital_io.in[6]==Digital_in::_0);

	controller_auto.update(gunner_joystick.button[Gamepad_button::START]);
	cout<<"controller_auto: "<<controller_auto<<"\n";
	if (!oi_panel.in_use || (oi_panel.in_use && oi_panel.collector_auto) || controller_auto.get()) {
		if(main_joystick.button[Gamepad_button::BACK])collector_mode=Collector_mode::NOTHING;
		else if(main_joystick.button[Gamepad_button::START]) {
			collector_mode=(toplevel_status.tilt.type() == Tilt::Status_detail::Type::TOP) ? (has_ball ? Collector_mode::REFLECT : Collector_mode::COLLECT) : Collector_mode::STOW;
		}
		cout<<"collector_mode: "<<collector_mode<<"\n";
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
			if(gunner_joystick.button[Gamepad_button::Y]) return Front::Goal::OUT;
			if(oi_panel.in_use) {
				#define X(name) if(oi_panel.front==Panel::Collector::name) return Front::Goal::name;
				X(IN) X(OUT) X(OFF)
				#undef X
				assert(0);
			}
			return Front::Goal::OFF;
		}();
		goals.sides=[&]{
			if(gunner_joystick.button[Gamepad_button::X]) return Sides::Goal::IN;
			if(gunner_joystick.button[Gamepad_button::B]) return Sides::Goal::OUT;
			if(oi_panel.in_use){
				#define X(name) if(oi_panel.sides==Panel::Collector::name) return Sides::Goal::name;
				X(IN) X(OUT) X(OFF)
				#undef X
				assert(0);
			}
			return Sides::Goal::OFF;
		}();
		goals.tilt=[&]{
			#define X(name,bt) bool name=gunner_joystick.button[Gamepad_button::bt];
			X(down,LB) X(up,RB) X(stop,BACK) X(learn,START) X(level,R_JOY)
			#undef X
			if(learn){
				#define X(button,mode) if(button)tilt_learn(toplevel_status.tilt.get_angle(),Tilt::Goal::Mode::mode);
				X(down,DOWN) X(up,UP) X(level,LEVEL)
				#undef X
			}
			if(down) return Tilt::Goal::down();
			if(up) return Tilt::Goal::up();
			if(stop) return Tilt::Goal::stop();
			if(level) return Tilt::Goal::level();
			if(oi_panel.in_use){
				switch(oi_panel.tilt){
					case Panel::Tilt::UP: return Tilt::Goal::up();
					case Panel::Tilt::DOWN: return Tilt::Goal::down();
					case Panel::Tilt::STOP: return Tilt::Goal::stop();
					default: assert(0);
				}
				if(oi_panel.control_angle)return Tilt::Goal::go_to_angle(make_tolerances(oi_panel.angle));
			}
			return goals.tilt;
		}();
		/*goals.climb=[&]{
			if(oi_panel.in_use){
				#define X(name) if(oi_panel.climber==Panel::Climber::name) return Climb::Goal::name;
				X(EXTEND) X(STOP) X(RETRACT)
				#undef X
				assert(0);	
			}
			return Climb::Goal::STOP;
		}();*/
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
				myfile2 << "SS: " << since_switch << endl;
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

	/*if(!oi_panel.in_use && (!in.robot_mode.enabled || in.robot_mode.autonomous)){
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
	
	//cout<<"panel: "<<oi_panel<<"\n";	
		
	bool autonomous_start_now=autonomous_start(in.robot_mode.autonomous && in.robot_mode.enabled);
	since_auto_start.update(in.now,autonomous_start_now);
		
	Toplevel::Goal goals;
	//Drivebase::Status_detail status_detail = drivebase.estimator.get();
	
	switch(mode){
		case Mode::TELEOP:
			goals=teleop(in,main_joystick,gunner_joystick,oi_panel,toplevel_status);
//test
			//tagThis("Line 347: switch(mode) teleop", __FILE__);
			//cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$tag!";
//test			
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
