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

const string LOG_PATH=[&]{
	string s;
	static const string NON_TEST_PATH="/home/lvuser/logs/";
	#ifndef MAIN_TEST
	s=NON_TEST_PATH;
	#endif
	return s;
}();

ofstream myfile2;

static int print_count=0;
#define SLOW_PRINT (print_count%10==0)

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
Main::Main():mode(Mode::TELEOP),autonomous_start(0),joy_collector_pos(Joy_collector_pos::STOP),collector_mode(Collector_mode::NOTHING){
		stepcounter=0;
		//the information that is being declared are just place holders for when we get and actual values for auto.
		s1.ptone.x=70;
		s1.ptone.y=70;
		s1.pttwo.x=70;
		s1.pttwo.y=60;
		s1.dirone=LEFT;
		s1.dirtwo=LEFT;
		
		s2.ptone.x=70;
		s2.ptone.y=70;
		s2.pttwo.x=70;
		s2.pttwo.y=60;
		s2.dirone=LEFT;
		s2.dirtwo=LEFT;

		s3.ptone.x=70;
		s3.ptone.y=70;
		s3.pttwo.x=70;
		s3.pttwo.y=60;
		s3.dirone=LEFT;
		s3.dirtwo=LEFT;
		
		s4.ptone.x=70;
		s4.ptone.y=70;
		s4.pttwo.x=70;
		s4.pttwo.y=60;
		s4.dirone=LEFT;
		s4.dirtwo=LEFT;
		
}


vector<Main::NavS> Main::loadnav(navloadinput navin){

	const string MYFILE=LOG_PATH+"navlog.txt";

	float amount = 0;
	vector<NavS> nav;
	NavS navelement;
	navinput start, end;
	vector<pair<int,movedir>> v;
	ofstream myfile(MYFILE);
	//oiload oinav;

	myfile << "hi" << "\n";
	myfile.flush();
	//assign start information
	start.navpt.x = navin.ptone.x;
	start.navpt.y = navin.ptone.y;
	start.navdir = navin.dirone;
	
	//assign end information
	end.navpt.x = navin.pttwo.x;
	end.navpt.y = navin.pttwo.y;
	end.navdir = navin.dirtwo;
	
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
		else 
			assert(0);

		navelement.amount = amount;
 
		myfile << "Pushing " << "navelm.left " << navelement.left << " navelm.right " << navelement.right << " amount " << navelement.amount << endl;
		nav.push_back(navelement);

		// push a delay
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
	static const float MAX_SPEED=1;//Change this value to change the max speed the robot will achieve with full boost (cannot be larger than 1.0)
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
	Panel const&  panel,
	Toplevel::Status_detail& toplevel_status
){
	Toplevel::Goal goals;

	bool spin=fabs(main_joystick.axis[Gamepad_axis::RIGHTX])>.01;
	double boost=main_joystick.axis[Gamepad_axis::LTRIGGER],slow=main_joystick.axis[Gamepad_axis::RTRIGGER];//spin, turbo, and slow buttons	
	
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
	
	bool ball=(in.digital_io.in[6]==Digital_in::_0);
	if(SLOW_PRINT){
		cout<<"Ball:"<<(ball? "has_ball" : "does not have ball")<<"\n";
		cout<<"controller_auto: "<<controller_auto.get()<<"\n";
	}
	main_panel_output[Panel_outputs::BOULDER] = Panel_output(Panel_output_ports::PBOULDER, ball);
	controller_auto.update(gunner_joystick.button[Gamepad_button::START]);
	if((!panel.in_use && controller_auto.get()) || (panel.in_use /*&& panel.collector_auto*/)) {
		if(main_joystick.button[Gamepad_button::BACK])collector_mode=Collector_mode::NOTHING;
		else if(main_joystick.button[Gamepad_button::START])collector_mode=(toplevel_status.tilt.type() == Tilt::Status_detail::Type::TOP) ? Collector_mode::COLLECT : Collector_mode::STOW;
		else if(gunner_joystick.button[Gamepad_button::A])collector_mode=Collector_mode::LOW_BAR;
		else if(gunner_joystick.button[Gamepad_button::X])collector_mode=Collector_mode::TERRAIN;
		else if(gunner_joystick.button[Gamepad_button::Y])collector_mode=Collector_mode::EJECT;
		else if(gunner_joystick.button[Gamepad_button::BACK])collector_mode=Collector_mode::NOTHING;
		if(collector_mode==Collector_mode::COLLECT && ball) collector_mode = Collector_mode::REFLECT;
		if(SLOW_PRINT)cout<<"collector_mode: "<<collector_mode<<"\n";
		switch(collector_mode){
			case Collector_mode::COLLECT:
				goals.front=Front::Goal::IN;
				goals.sides=Sides::Goal::IN;
				goals.tilt=Tilt::Goal::level();
				break;
			case Collector_mode::STOW:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::up();
				break;
			case Collector_mode::REFLECT:
				goals.front=Front::Goal::OUT;
				goals.sides=Sides::Goal::OUT;
				goals.tilt=Tilt::Goal::level();
				break;
			case Collector_mode::EJECT:
				goals.front=Front::Goal::OUT;
				goals.sides=Sides::Goal::IN;
				goals.tilt=Tilt::Goal::level();
				break;
			case Collector_mode::TERRAIN:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::up();
				break;
			case Collector_mode::LOW_BAR:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::low();
				break;
			case Collector_mode::NOTHING:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::stop();
				break;
			default: assert(0);
		}
	} else {
		goals.front=[&]{
			const double LIMIT = .5; 
			if(gunner_joystick.axis[Gamepad_axis::LTRIGGER]>LIMIT) return Front::Goal::OUT;
			if(gunner_joystick.axis[Gamepad_axis::RTRIGGER]>LIMIT) return Front::Goal::IN;
			if(panel.in_use) {
				#define X(name) if(panel.front==Panel::Collector::name) return Front::Goal::name;
				X(IN) X(OUT) X(OFF)
				#undef X
				assert(0);
			}
			return Front::Goal::OFF;
		}();
		goals.sides=[&]{
			if(gunner_joystick.button[Gamepad_button::RB]) return Sides::Goal::IN;
			if(gunner_joystick.button[Gamepad_button::LB]) return Sides::Goal::OUT;
			if(panel.in_use){
				#define X(name) if(panel.sides==Panel::Collector::name) return Sides::Goal::name;
				X(IN) X(OUT) X(OFF)
				#undef X
				assert(0);
			}
			return Sides::Goal::OFF;
		}();
		goals.tilt=[&]{
			{
				Joystick_section tilt_control = joystick_section(gunner_joystick.axis[Gamepad_axis::RIGHTX],gunner_joystick.axis[Gamepad_axis::RIGHTY]);
				#define X(name,section) bool name=tilt_control==Joystick_section::section;
				X(down,DOWN) X(up,UP) X(level,LEFT) X(low,RIGHT)
				#undef X		
				if(low) joy_collector_pos = Joy_collector_pos::LOW;
				else if(level) joy_collector_pos = Joy_collector_pos::LEVEL;
				if(gunner_joystick.button[Gamepad_button::B]){//learn
					#define LEARN(button,mode) if(button)tilt_learn(toplevel_status.tilt.pot_value(),""#mode);
					LEARN(down,BOTTOM) LEARN(level,LEVEL) LEARN(low,LOW)
					#undef LEARN
					joy_collector_pos = Joy_collector_pos::STOP;
				} else {
					Joy_collector_pos last = joy_collector_pos;
					joy_collector_pos = Joy_collector_pos::STOP;
					if(down) return Tilt::Goal::down();
					if(up) return Tilt::Goal::up();
					joy_collector_pos = last;
				}
			}
			switch (joy_collector_pos) {
				case Joy_collector_pos::STOP: return Tilt::Goal::stop();
				case Joy_collector_pos::LOW: return Tilt::Goal::low();
				case Joy_collector_pos::LEVEL: return Tilt::Goal::level();
				default: assert(0);
			}
		}();
		goals.tilt.force_down = gunner_joystick.button[Gamepad_button::R_JOY];
	}
	goals.climb_release=[&]{
		if(main_joystick.button[Gamepad_button::LB])return Climb_release::Goal::IN;
		if(main_joystick.button[Gamepad_button::RB])return Climb_release::Goal::OUT;
		return Climb_release::Goal::STOP;
	}();
	goals.winch=[&]{
		switch (joystick_section(gunner_joystick.axis[Gamepad_axis::LEFTX], gunner_joystick.axis[Gamepad_axis::LEFTY])){
			case Joystick_section::UP: return Winch::Goal::OUT;
			case Joystick_section::DOWN: return Winch::Goal::IN;
			default: break;
		}
		if(panel.in_use){
			switch(panel.winch){
				case Panel::Winch::UP: return Winch::Goal::OUT;
				case Panel::Winch::DOWN: return Winch::Goal::IN;
				case Panel::Winch::STOP: return Winch::Goal::STOP;
				default: assert(0);
			}
		}
		return Winch::Goal::STOP;
	}();
	return goals;
}

Main::Mode next_mode(Main::Mode m,bool autonomous,bool autonomous_start,Toplevel::Status_detail /*status*/,Time since_switch, Panel /*panel*/,unsigned int navindex,std::vector<Main::NavS> NavV,int & stepcounter,Main::aturn Aturn){
	switch(m){
		case Main::Mode::TELEOP:	
			if(autonomous_start){
				return Main::Mode::AUTO_NAV;//just for testing purposes
				stepcounter = 1; //this needs to be moved to where the oi sets the mode later.
				/*if (panel.in_use) {
					switch(panel.auto_mode){ 
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

		case Main::Mode::AUTO_NULL:
			if(autonomous)
			cout << "test" << endl;
			return Main::Mode::TELEOP;
		
		case Main::Mode::AUTO_NAV:
			if(stepcounter==1 /* || stepcounter==2*/)
				return Main::Mode::AUTO_NAV_RUN;
			else if(stepcounter==2)
				return Main::Mode::TELEOP;
			else if(stepcounter==3)
				return Main::Mode::AUTO_MOVE;
			else if(stepcounter==4)  
				return Main::Mode::AUTO_SCORE;
			else if(stepcounter==5)	
				return Main::Mode::TELEOP;

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
				return Main::Mode::AUTO_NAV;
			}
			return Main::Mode::AUTO_NAV_RUN;

		case Main::Mode::AUTO_MOVE:
			//encoders? going to use time for now
			if(since_switch>Aturn.dur) 
				return Main::Mode::AUTO_NAV;
			return Main::Mode::AUTO_MOVE;

		case Main::Mode::AUTO_SCORE:
			if(since_switch>1) 
				return Main::Mode::AUTO_NAV;
			return Main::Mode::AUTO_MOVE;

		default: assert(0);
	}
	return m;
	
}

Robot_outputs Main::operator()(Robot_inputs in,ostream&){
	print_count++;

	perf.update(in.now);
	Joystick_data main_joystick=in.joystick[0];
	Joystick_data gunner_joystick=in.joystick[1];
	Panel panel=interpret(in.joystick[2]);

	/*if(!panel.in_use && (!in.robot_mode.enabled || in.robot_mode.autonomous)){
		Panel empty;
		panel=empty;
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
		
	if (SLOW_PRINT) cout<<"panel: "<<panel<<"\n";	
		
	bool autonomous_start_now=autonomous_start(in.robot_mode.autonomous && in.robot_mode.enabled);
	since_auto_start.update(in.now,autonomous_start_now);
		
	Toplevel::Goal goals;
	//Drivebase::Status_detail status_detail = drivebase.estimator.get();
	
	switch(mode){
		case Mode::TELEOP:
			goals=teleop(in,main_joystick,gunner_joystick,panel,toplevel_status);
			//test
			//tagThis("Line 347: switch(mode) teleop", __FILE__);
			//cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$tag!";
			//test			
			break;
		case Mode::AUTO_NULL:
			break;
		case Mode::AUTO_NAV:
			
			myfile2.open(LOG_PATH+"navlog2.txt");

			if(stepcounter==1)
				NavV = loadnav(s1);
			else if(stepcounter==2)
				NavV = loadnav(s2);
			else if(stepcounter==3){
				Aturn.l = -.45;
				Aturn.r = .45;
				Aturn.dur=1;
				break;
			}
			else if(stepcounter==4)
				//fire the ball!
				break;
			else if(stepcounter==5)
				//all done
				break;
			/*
			else if(stepcounter==5)
			
				break;
			else if(stepcounter==6)
				NavV = loadnav(s3);
			else if(stepcounter==7)
				NavV = loadnav(s4);
			*/
			else
				assert(0);

			navindex = 0;
			myfile2 << "Nav loaded:" << NavV.size() << endl;
			myfile2.flush();
			break;

		case Mode::AUTO_NAV_RUN:
			goals.drive.left=NavV[navindex].left;
			goals.drive.right=NavV[navindex].right;
			break;

		case Mode::AUTO_MOVE:
			goals.drive.left=Aturn.l;
			goals.drive.right=Aturn.r;
			break;
		case Mode::AUTO_SCORE:
			//score on low goal.
			break;
		default: assert(0);
	}
	auto next=next_mode(mode,in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,navindex,NavV,stepcounter,Aturn);
	since_switch.update(in.now,mode!=next);
	mode=next;
	
	Toplevel::Output r_out=control(toplevel_status,goals); 
	auto r=toplevel.output_applicator(Robot_outputs{},r_out);
	
	r.panel_output = main_panel_output;	
	
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
