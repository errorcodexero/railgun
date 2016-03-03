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
	#ifndef MAIN_TEST
	const string NON_TEST_PATH="/home/lvuser/logs/";
	s=NON_TEST_PATH;
	#endif
	return s;
}();

ofstream myfile2;

static int print_count=0;
#define SLOW_PRINT (print_count%10==0)

//TODO: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():mode(Mode::TELEOP),autonomous_start(0),joy_collector_pos(Joy_collector_pos::STOP),collector_mode(Collector_mode::NOTHING){}

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

vector<Main::NavS> Main::loadnav(){
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
	start.navpt.x = 70;
	start.navpt.y = 70;
	start.navdir = LEFT;
	
	//assign end information
	end.navpt.x = 70;
	end.navpt.y = 60;
	end.navdir = LEFT;
	
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

double set_drive_speed(double axis,double boost,double slow=0){
	static const float MAX_SPEED=1;//Change this value to change the max speed the robot will achieve with full boost (cannot be larger than 1.0)
	static const float DEFAULT_SPEED=.3;//Change this value to change the default speed
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
	(void)toplevel_status;
	Toplevel::Goal goals;

	bool spin=fabs(main_joystick.axis[Gamepad_axis::RIGHTX])>.01;//drive turning button
	double boost=main_joystick.axis[Gamepad_axis::LTRIGGER],slow=main_joystick.axis[Gamepad_axis::RTRIGGER];//turbo and slow buttons	
	
	static const double NUDGE_POWER=.4,NUDGE_CW_POWER=.4,NUDGE_CCW_POWER=-.4; 
	goals.drive.left=[&]{
		double power=set_drive_speed(main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
		if(spin) power+=set_drive_speed(-main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
		if(!nudges[Nudges::FORWARD].timer.done()) power=-NUDGE_POWER;
		else if(!nudges[Nudges::BACKWARD].timer.done()) power=NUDGE_POWER;
		else if(!nudges[Nudges::CLOCKWISE].timer.done()) power=-NUDGE_CW_POWER;
		else if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) power=-NUDGE_CCW_POWER;
		return clip(power);
	}();
	goals.drive.right=[&]{
		double power=set_drive_speed(main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
		if(spin) power+=set_drive_speed(main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
		if(!nudges[Nudges::FORWARD].timer.done()) power=-NUDGE_POWER;
		else if(!nudges[Nudges::BACKWARD].timer.done()) power=NUDGE_POWER;
		else if(!nudges[Nudges::CLOCKWISE].timer.done()) power=NUDGE_CW_POWER;
		else if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) power=NUDGE_CCW_POWER;
		return clip(power);
	}();

	static const unsigned int nudge_buttons[NUDGES]={Gamepad_button::Y,Gamepad_button::A,Gamepad_button::B,Gamepad_button::X};//Forward, backward, clockwise, counter-clockwise
	for(int i=0;i<Nudges::NUDGES;i++){
		if(nudges[i].trigger(boost<.25 && main_joystick.button[nudge_buttons[i]])) nudges[i].timer.set(.1);
		nudges[i].timer.update(in.now,1);
	}
	
	bool ball=(in.digital_io.in[6]==Digital_in::_0);
	
	const Tilt::Goal LEVEL=Tilt::Goal::go_to_angle({83,83,83});
	const Tilt::Goal LOW=Tilt::Goal::go_to_angle({100,100,100});
	const Tilt::Goal TOP=Tilt::Goal::go_to_angle({0,0,0});
	
	if((!panel.in_use && controller_auto.get()) || (panel.in_use && (panel.tilt_auto || panel.front_auto || panel.sides_auto))) {
	main_panel_output[Panel_outputs::BOULDER] = Panel_output(static_cast<int>(Panel_output_ports::BOULDER), ball);
	
	controller_auto.update(gunner_joystick.button[Gamepad_button::START]);
		if(main_joystick.button[Gamepad_button::BACK]) collector_mode=Collector_mode::NOTHING;
		else if(main_joystick.button[Gamepad_button::START] || (panel.in_use && panel.collect)) collector_mode=Collector_mode::COLLECT;
		else if(gunner_joystick.button[Gamepad_button::A] || (panel.in_use && panel.collector_pos==Panel::Collector_pos::LOW)) collector_mode=Collector_mode::LOW;
		else if(gunner_joystick.button[Gamepad_button::X] || (panel.in_use && panel.shoot)) {
			collector_mode=Collector_mode::SHOOT;
			shoot_timer.set(1);
		}
		else if(gunner_joystick.button[Gamepad_button::Y] || (panel.in_use && panel.eject)) collector_mode=Collector_mode::EJECT;
		else if(gunner_joystick.button[Gamepad_button::BACK]) collector_mode=Collector_mode::NOTHING;
		else if(panel.in_use && panel.collector_pos==Panel::Collector_pos::STOW) collector_mode = Collector_mode::STOW;
		if(SLOW_PRINT)cout<<"collector_mode: "<<collector_mode<<"\n";
		switch(collector_mode){
			case Collector_mode::COLLECT:
				goals.front=Front::Goal::IN;
				goals.sides=Sides::Goal::IN;
				goals.tilt=LEVEL;
				if(ball) collector_mode=Collector_mode::STOW;
				break;
			case Collector_mode::STOW:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::up();
				break;
			case Collector_mode::REFLECT:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OUT;
				goals.tilt=LEVEL;
				break;
			case Collector_mode::EJECT:
				goals.front=Front::Goal::OUT;
				goals.sides=Sides::Goal::IN;
				goals.tilt=LEVEL;
				break;
			case Collector_mode::SHOOT:
				goals.front=Front::Goal::OUT;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=TOP;
				shoot_timer.update(in.now, true);
				if (shoot_timer.done()) collector_mode = Collector_mode::STOW;
				break;
			case Collector_mode::LOW:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=LOW;;
				break;
			case Collector_mode::NOTHING:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::stop();
				break;
			default: assert(0);
		}
	}
	if (!panel.in_use && !controller_auto.get()) { 
		goals.front=[&]{
			const double LIMIT=.5; 
			if(gunner_joystick.axis[Gamepad_axis::LTRIGGER]>LIMIT) return Front::Goal::OUT;
			if(gunner_joystick.axis[Gamepad_axis::RTRIGGER]>LIMIT) return Front::Goal::IN;
			return Front::Goal::OFF;
		}();
		goals.sides=[&]{
			if(gunner_joystick.button[Gamepad_button::RB]) return Sides::Goal::IN;
			if(gunner_joystick.button[Gamepad_button::LB]) return Sides::Goal::OUT;
			return Sides::Goal::OFF;
		}();
		#if 0
		goals.tilt=[&]{
                        {
                                Joystick_section tilt_control = joystick_section(gunner_joystick.axis[Gamepad_axis::RIGHTX],gunner_joystick.axis[Gamepad_axis::RIGHTY]);
                                #define X(name,section) bool name=tilt_control==Joystick_section::section;
                                X(down,DOWN) X(up,UP) X(level,LEFT) X(low,RIGHT)
                                #undef X                
                                if(low) joy_collector_pos = Joy_collector_pos::LOW;
                                else if(level) joy_collector_pos = Joy_collector_pos::LEVEL;
                                if(gunner_joystick.button[Gamepad_button::B]){//learn
                                        /*#define LEARN(button,mode) if(button)tilt_learn(toplevel_status.tilt.pot_value(),""#mode);
                                        LEARN(down,BOTTOM) LEARN(level,LEVEL) LEARN(low,LOW)
                                        #undef LEARN
                                        joy_collector_pos = Joy_collector_pos::STOP;*/
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
                                case Joy_collector_pos::LOW: return LOW;
                                case Joy_collector_pos::LEVEL: return LEVEL;
                                default: assert(0);
                        }
                }();	
		#endif
	}
	if(gunner_joystick.button[Gamepad_button::BACK]){
		goals.tilt=Tilt::Goal::stop();
		goals.sides=Sides::Goal::OFF;
		goals.front=Front::Goal::OFF;
	}
	if (panel.in_use) {
		if (!panel.front_auto) {
			#define X(name) if(panel.front==Panel::Collector::name) goals.front = Front::Goal::name;
			X(IN) X(OUT) X(OFF)
			#undef X
		}
		if (!panel.sides_auto) {
			#define X(name) if(panel.sides==Panel::Collector::name) goals.sides = Sides::Goal::name;
			X(IN) X(OFF) X(OUT)
			#undef X
		}
		if (!panel.tilt_auto) {
                        goals.tilt=[&]{
				if (panel.collector_up) return Tilt::Goal::up();
                        	if (panel.collector_down) return Tilt::Goal::down();
				return goals.tilt=Tilt::Goal::stop();
			}();
                }
	}
	goals.climb_release=[&]{
		if(main_joystick.button[Gamepad_button::LB])return Climb_release::Goal::IN;
		if(main_joystick.button[Gamepad_button::RB])return Climb_release::Goal::OUT;
		if(panel.in_use) {
			if(panel.lock_climber) return Climb_release::Goal::OUT;
			return Climb_release::Goal::IN;
		}
		return Climb_release::Goal::STOP;
	}();
	goals.winch=[&]{
		switch (joystick_section(gunner_joystick.axis[Gamepad_axis::LEFTX], gunner_joystick.axis[Gamepad_axis::LEFTY])){
			case Joystick_section::UP: return Winch::Goal::OUT;
			case Joystick_section::DOWN: return Winch::Goal::IN;
			default: break;
		}
		if(panel.in_use && !panel.lock_climber){
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

Main::Mode next_mode(Main::Mode m,bool autonomous,bool autonomous_start,Toplevel::Status_detail /*status*/,Time since_switch, Panel /*panel*/,unsigned int navindex,std::vector<Main::NavS> NavV){
	switch(m){
		case Main::Mode::TELEOP:	
			if(autonomous_start){
				return Main::Mode::AUTO_NAV;//just for testing purposes
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
		case Main::Mode::AUTO_MOVE:
			//encoders? going to use time for now
			if(!autonomous || since_switch>1) return Main::Mode::TELEOP;
			return m;
		case Main::Mode::AUTO_NAV:
			return Main::Mode::AUTO_NAV_RUN;
		case Main::Mode::AUTO_NAV_RUN:
			if(since_switch>NavV[navindex].amount) {
				navindex++;
				myfile2 << "navindex:" << navindex << endl << "SS: " << since_switch << endl;
				myfile2.flush();
			} 
			if(navindex==NavV.size()) {
				myfile2 << "done" << endl;
				myfile2.flush();
				return Main::Mode::TELEOP;
			}
			return Main::Mode::AUTO_NAV_RUN;
		case Main::Mode::AUTO_NAV_DAMAGE_DRIVE:
		case Main::Mode::AUTO_NAV_DAMAGE_MANIPULATOR:
		default: assert(0);
	}
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
		
	if(SLOW_PRINT) cout<<"panel: "<<panel<<"\n";	
		
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
		case Mode::AUTO_MOVE:
			goals.drive.left=.45;
			goals.drive.right=.45;
			break;
		case Mode::AUTO_NAV:
			{
				const string MYFILE2=LOG_PATH+"navlog2.txt";
				myfile2.open(MYFILE2);
				NavV = loadnav();
				navindex = 0;
				myfile2 << "Nav loaded:" << NavV.size() << endl;
				myfile2.flush();
				break;
			}
		case Mode::AUTO_NAV_RUN:
			goals.drive.left=NavV[navindex].left;
			goals.drive.right=NavV[navindex].right;
			break;
		case Mode::AUTO_NAV_DAMAGE_DRIVE:
			break;
		case Mode::AUTO_NAV_DAMAGE_MANIPULATOR:
			break;
		default: assert(0);
	}
	auto next=next_mode(mode,in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,navindex,NavV);
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
