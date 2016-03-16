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

const string FILE_PATH=[&]{
	string s;
	#ifndef MAIN_TEST
	const string NON_TEST_PATH="/home/lvuser/";
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

ostream& operator<<(ostream& o,Main::Cheval_steps a){
	#define X(NAME) if(a==Main::Cheval_steps::NAME) return o<<"Main::Cheval_steps("#NAME")";
	CHEVAL_STEPS
	#undef X
	assert(0);
}

Tilt_presets::Tilt_presets(){
		top=0;
		level=83;
		low=100;
		cheval=100;
		portcullis=83;
}

#define PRESETS X(top) X(level) X(low) X(cheval) X(portcullis)

bool operator==(Tilt_presets const& a,Tilt_presets const& b){
	#define X(NAME) if(a.NAME!=b.NAME) return 0;
	PRESETS
	#undef X
	return 1;
}

ostream& operator<<(ostream& o,Tilt_presets const& a){
	return o<<"Presets( top:"<<a.top<<"  level:"<<a.level<<"  low:"<<a.low<<"  cheval:"<<a.cheval<<"   portcullis:"<<a.portcullis<<")";
}

#ifdef MAIN_TEST
static const auto PRESET_FILE="presets1.txt";
#else
static const auto PRESET_FILE="/home/lvuser/presets1.txt";
#endif

void write_tilt_presets(Tilt_presets const& a){
	ofstream o(PRESET_FILE);
	if(!o.good()){
		cerr<<"Error: could not open file to write presets\n";
		return;
	}
	#define X(NAME) o<<a.NAME<<"\n";
	PRESETS
	#undef X
}

Tilt_presets read_tilt_presets(){
	Tilt_presets r;
	ifstream f(PRESET_FILE);
	if(!f.good()){
 		//if can't open file then just return the defaults
		cerr<<"Error: could not open preset file.  Using defaults.\n";
		return r;
	}
	string s;
	#define X(NAME){ getline(f,s); r.NAME=atof(s.c_str()); }
	PRESETS
	#undef X
	return r;
}

//TODO: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():mode(Mode::TELEOP),autonomous_start(0),joy_collector_pos(Joy_collector_pos::STOP),collector_mode(Collector_mode::NOTHING),cheval_step(Cheval_steps::GO_DOWN){
	myfile2.open("/home/lvuser/navlogs/navlog2.txt");
	myfile2 << "test start" << endl;
	tilt_presets=read_tilt_presets();
}


double set_drive_speed(double axis,double boost,double slow=0){
	static const float MAX_SPEED=1;//Change this value to change the max speed the robot will achieve with full boost (cannot be larger than 1.0)
	static const float DEFAULT_SPEED=.4;//Change this value to change the default speed
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
	Panel const& panel,
	Toplevel::Status_detail& toplevel_status,
	Tilt::Goal level,
	Tilt::Goal low,
	Tilt::Goal top,
	Tilt::Goal cheval,
	Tilt::Goal portcullis
){
	Toplevel::Goal goals;
	
	bool enabled = in.robot_mode.enabled;
	
	{//Set drive goals
		bool spin=fabs(main_joystick.axis[Gamepad_axis::RIGHTX])>.01;//drive turning button
		double boost=main_joystick.axis[Gamepad_axis::LTRIGGER],slow=main_joystick.axis[Gamepad_axis::RTRIGGER];//turbo and slow buttons	
	
		for(int i=0;i<NUDGES;i++){
			const array<unsigned int,NUDGES> nudge_buttons={Gamepad_button::Y,Gamepad_button::A,Gamepad_button::B,Gamepad_button::X};//Forward, backward, clockwise, counter-clockwise
			if(nudges[i].trigger(boost<.25 && main_joystick.button[nudge_buttons[i]])) nudges[i].timer.set(.1);
			nudges[i].timer.update(in.now,enabled);
		}
		const double NUDGE_POWER=.4,NUDGE_CW_POWER=.4,NUDGE_CCW_POWER=-.4; 
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
	}
	
	bool ball=(in.digital_io.in[6]==Digital_in::_0);
	main_panel_output[Panel_outputs::BOULDER] = Panel_output(static_cast<int>(Panel_output_ports::BOULDER), ball);
	
	controller_auto.update(gunner_joystick.button[Gamepad_button::START]);

	if(gunner_joystick.button[Gamepad_button::BACK] || main_joystick.button[Gamepad_button::BACK]){//Turn collector off
		goals.tilt=Tilt::Goal::stop();
		goals.sides=Sides::Goal::OFF;
		goals.front=Front::Goal::OFF;
		collector_mode=Collector_mode::NOTHING;
	}	
	bool learning=(learn.get() || !learn_delay.done());
	main_panel_output[Panel_outputs::LEARNING] = Panel_output(static_cast<int>(Panel_output_ports::LEARNING), learning);
	//if(SLOW_PRINT) cout<<"learning("<<learning<<")   delaying("<<!learn_delay.done()<<")\n";
	
	if(SLOW_PRINT) cout<<tilt_presets<<"\n";
	
	if((!panel.in_use && controller_auto.get()) || (panel.in_use && (panel.tilt_auto || panel.front_auto || panel.sides_auto))) {//Automatic collector modes
		bool joy_learn=gunner_joystick.button[Gamepad_button::B];
		POV_section gunner_pov = pov_section(gunner_joystick.pov);
		if(gunner_joystick.button[Gamepad_button::X] || (panel.in_use && panel.shoot)) {
			collector_mode=Collector_mode::SHOOT;
			const Time TIME_TO_SHOOT=.7;
			shoot_timer.set(TIME_TO_SHOOT);
			
		}
		else if((gunner_pov==POV_section::UP && !joy_learn) || (panel.in_use && panel.collector_pos==Panel::Collector_pos::STOW && !learning)) collector_mode = Collector_mode::STOW;
		else if((gunner_pov==POV_section::RIGHT && !joy_learn) || (panel.in_use && panel.cheval && !learning)) {
			collector_mode = Collector_mode::CHEVAL;
			cheval_lift_timer.set(.45);
			cheval_drive_timer.set(2);
			cheval_step = Cheval_steps::GO_DOWN;
		}
		else if((gunner_pov==POV_section::LEFT && !joy_learn) || (panel.in_use && panel.portcullis && !learning)){
			collector_mode = Collector_mode::PORTCULLIS;
			const Time TIME_UNTIL_OVER=1;
			portcullis_timer.set(TIME_UNTIL_OVER);	
		}
		else if((gunner_joystick.button[Gamepad_button::Y] && !joy_learn) || (panel.in_use && panel.eject && !learning)) collector_mode=Collector_mode::EJECT;
		else if((main_joystick.button[Gamepad_button::START] && !joy_learn) || (panel.in_use && panel.collect && !learning)) collector_mode=Collector_mode::COLLECT;
		else if((gunner_joystick.button[Gamepad_button::A] && !joy_learn) || (panel.in_use && panel.collector_pos==Panel::Collector_pos::LOW && !learning)) collector_mode=Collector_mode::LOW;
		if(SLOW_PRINT)cout<<"collector_mode: "<<collector_mode<<"\n";
		switch(collector_mode){
			case Collector_mode::COLLECT:
				goals.front=Front::Goal::IN;
				goals.sides=Sides::Goal::IN;
				goals.tilt=level;
				if(ball) collector_mode=Collector_mode::STOW;
				break;
			case Collector_mode::STOW:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=top;
				break;
			case Collector_mode::REFLECT:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OUT;
				goals.tilt=level;
				break;
			case Collector_mode::EJECT:
				goals.front=Front::Goal::OUT;
				goals.sides=Sides::Goal::IN;
				goals.tilt=level;
				break;
			case Collector_mode::SHOOT:
				goals.front=Front::Goal::OUT;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=top;
				shoot_timer.update(in.now, enabled);
				if (shoot_timer.done()) collector_mode = Collector_mode::STOW;
				break;
			case Collector_mode::LOW:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=low;
				break;
			case Collector_mode::NOTHING:
				goals.front=Front::Goal::OFF;
				goals.sides=Sides::Goal::OFF;
				goals.tilt=Tilt::Goal::stop();
				break;
			case Collector_mode::CHEVAL:
				{
					const double AUTO_POWER=-.5;
					goals.front=Front::Goal::OFF;
					goals.sides=Sides::Goal::OFF;
					switch(cheval_step) {
						case Cheval_steps::GO_DOWN: 
							goals.tilt=cheval;
							if(ready(status(toplevel_status.tilt),goals.tilt)) cheval_step=Cheval_steps::DRIVE;
							break;
						case Cheval_steps::DRIVE:
							goals.drive.right=AUTO_POWER;
							goals.drive.left=AUTO_POWER;
							cheval_lift_timer.update(in.now,enabled);
							if (cheval_lift_timer.done()) cheval_step=Cheval_steps::DRIVE_AND_STOW;
							break;
						case Cheval_steps::DRIVE_AND_STOW:
							goals.drive.right=AUTO_POWER;
							goals.drive.left=AUTO_POWER;
							goals.tilt=top;
							cheval_drive_timer.update(in.now,enabled);
							if (cheval_drive_timer.done()) collector_mode=Collector_mode::STOW;
							break;
						default: 
							assert(0);
					}
					break;
				}
			case Collector_mode::PORTCULLIS:
				{
					portcullis_timer.update(in.now,enabled);
					goals.front=Front::Goal::OFF;
					goals.sides=Sides::Goal::OFF;
					goals.tilt=portcullis;
					const double AUTO_POWER=-.5;
					goals.drive.right=AUTO_POWER;
					goals.drive.left=AUTO_POWER;
					if(portcullis_timer.done()) collector_mode = Collector_mode::STOW;				
					break;
				}
			default: assert(0);
		}
	}
	
	if (!panel.in_use && !controller_auto.get()) {//Manual joystick controls 
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
		goals.tilt=[&]{
			{
				Joystick_section tilt_control = joystick_section(gunner_joystick.axis[Gamepad_axis::RIGHTX],gunner_joystick.axis[Gamepad_axis::RIGHTY]);
				#define X(NAME,SECTION) bool NAME=tilt_control==Joystick_section::SECTION;
				X(down_b,DOWN) X(up_b,UP) X(level_b,LEFT) X(low_b,RIGHT)
				#undef X		
				if(low_b) joy_collector_pos = Joy_collector_pos::LOW;
				else if(level_b) joy_collector_pos = Joy_collector_pos::LEVEL;
				if(gunner_joystick.button[Gamepad_button::B]){//learn
					double learn_this=toplevel_status.tilt.angle;
					switch(joy_collector_pos){
						case Joy_collector_pos::LOW:
							tilt_presets.low=learn_this;
							write_tilt_presets(tilt_presets);
							break;
						case Joy_collector_pos::LEVEL:
							tilt_presets.level=learn_this;
							write_tilt_presets(tilt_presets);
							break;
						default: break;
						
					}
				} else {
					Joy_collector_pos last = joy_collector_pos;
					joy_collector_pos = Joy_collector_pos::STOP;
					if(down_b) return Tilt::Goal::down();
					if(up_b) return Tilt::Goal::up();
					joy_collector_pos = last;
				}
			}
			switch (joy_collector_pos) {
				case Joy_collector_pos::STOP: return Tilt::Goal::stop();
				case Joy_collector_pos::LOW: return low;
				case Joy_collector_pos::LEVEL: return level;
				default: assert(0);
			}
		}();	
		//if(!panel.in_use) goals.tilt.percent_power=1.00;
	}
	learn_delay.update(in.now,enabled);
	if (panel.in_use) {//Panel manual modes
		//goals.tilt.percent_power=panel.speed_dial;
		learn.update(panel.learn);
		if(learn.get()){//learn
			double learn_this=toplevel_status.tilt.angle;
			bool done=false;
			if(panel.collect || panel.eject){
				tilt_presets.level=learn_this;
				done=true;
			}
			else if(panel.collector_pos==Panel::Collector_pos::STOW){
				tilt_presets.top=learn_this;
				done=true;
			}
			else if(panel.cheval){
				tilt_presets.cheval=learn_this;
				done=true;
			}
			else if(panel.portcullis){
				tilt_presets.portcullis=learn_this;
				done=true;
			}
			else if(panel.collector_pos==Panel::Collector_pos::LOW){
				tilt_presets.low=learn_this;
				done=true;
			}
			else learn.update(panel.learn);
			if(done){
				if(learn.get()) learn.update(true);
				learn_delay.set(.5);
				write_tilt_presets(tilt_presets);
			}
		}
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
		//if(main_joystick.button[Gamepad_button::LB]) return Climb_release::Goal::IN;
		//if(main_joystick.button[Gamepad_button::RB]) return Climb_release::Goal::OUT;
		if(panel.in_use) {
			if(panel.lock_climber) return Climb_release::Goal::OUT;
			return Climb_release::Goal::IN;
		}
		return Climb_release::Goal::OUT;
	}();
	goals.winch=[&]{
		switch (joystick_section(gunner_joystick.axis[Gamepad_axis::LEFTX], gunner_joystick.axis[Gamepad_axis::LEFTY])){
			case Joystick_section::UP: 
			case Joystick_section::DOWN: return Winch::Goal::IN;
			default: break;
		}
		if(panel.in_use && toplevel_status.climb_release==Climb_release::Status_detail::IN){
			switch(panel.winch){
				case Panel::Winch::UP: 
				case Panel::Winch::DOWN: return Winch::Goal::IN;	
				case Panel::Winch::STOP: return Winch::Goal::STOP;
				default: assert(0);
			}
		}
		return Winch::Goal::STOP;
	}();
	return goals;
}

Main::Mode next_mode(Main::Mode m,bool autonomous,bool autonomous_start,Toplevel::Status_detail /*status*/,Time since_switch, Panel panel,unsigned int navindex,vector<Nav2::NavS> NavV,int & stepcounter,Nav2::aturn Aturn){
	switch(m){
		case Main::Mode::TELEOP:	
			if(autonomous_start){
				myfile2 << "NEXT_MODE:AUTO_REACH***" << endl;
				//return Main::Mode::AUTO_STATIC;//just for testing purposes

				if (panel.in_use) {
					switch(panel.auto_mode){ 
						case Panel::Auto_mode::NOTHING:
							return Main::Mode::AUTO_NULL;
						case Panel::Auto_mode::REACH:
							return Main::Mode::AUTO_REACH;
						case Panel::Auto_mode::STATICS:
							return Main::Mode::AUTO_STATICTWO;
						case Panel::Auto_mode::STATICF:
							return Main::Mode::AUTO_STATIC;
						default: assert(0);
					}
				} else {
					return Main::Mode::AUTO_TEST; //during testing put the mode you want to test without the driverstation.
				}
				return Main::Mode::TELEOP;
			}
			return m;

		case Main::Mode::AUTO_NULL:
			return Main::Mode::TELEOP;
		
		case Main::Mode::AUTO_NAV:
			//if(stepcounter==1 /* || stepcounter==2*/){
				//stepcounter ++;
				if(!autonomous) return Main::Mode::TELEOP;
				return Main::Mode::AUTO_NAV_RUN;
			//}else if(stepcounter==2)
				//return Main::Mode::AUTO_MOVE;

			myfile2 << "AUTO_NAV:next:" << stepcounter << endl;


		case Main::Mode::AUTO_NAV_RUN:
		
			if(since_switch>NavV[navindex].amount) {
				navindex++;
				myfile2 << "navindex:" << navindex << endl << "SS: " << since_switch << endl;
				myfile2.flush();
			
			} 
			if(navindex==NavV.size()) {
				myfile2 << "done" << endl;
				myfile2.flush();
				return Main::Mode::AUTO_NAV;
			}
			return Main::Mode::AUTO_NAV_RUN;

		case Main::Mode::AUTO_MOVE:
			if (!autonomous) return Main::Mode::TELEOP;
			if(since_switch>Aturn.dur) return Main::Mode::AUTO_NAV;
			return Main::Mode::AUTO_MOVE;

		case Main::Mode::AUTO_SCORE:
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch>1) 
				return Main::Mode::AUTO_NAV;
			return Main::Mode::AUTO_MOVE;
 
		case Main::Mode::AUTO_REACH:
			myfile2 << "NEXT_MODE:SS" << since_switch << endl;
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch > .8) return Main::Mode::AUTO_STOP;
			return Main::Mode::AUTO_REACH;

		case Main::Mode::AUTO_STATIC:
			if (!autonomous) return Main::Mode::TELEOP;
			if(since_switch > 1.5) return Main::Mode::AUTO_STOP;
			return Main::Mode::AUTO_STATIC;
		case Main::Mode::AUTO_STATICTWO:
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch > 2.5) return Main::Mode::AUTO_STOP;
			return Main::Mode::AUTO_STATICTWO;

		case Main::Mode::AUTO_STOP:
			myfile2 << "NEXT_MODE:DONE=>TELEOP" << endl;
			return Main::Mode::TELEOP;
		case Main::Mode::AUTO_TEST:
			if(since_switch > 1 ||!autonomous) return Main::Mode::TELEOP;
			return Main::Mode::AUTO_TEST;
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

	Tilt::Goal level=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.level));
	Tilt::Goal low=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.low));
	Tilt::Goal top=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));
	Tilt::Goal cheval=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.cheval));
	Tilt::Goal portcullis=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.portcullis));

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
	/*if(SLOW_PRINT){
		cout << "ENCODER '0': " <<  in.digital_io.encoder[0]<<  endl;
		cout << "ENCODER '1': " << in.digital_io.encoder[1]<<  endl;	
	}*/
	switch(mode){
		case Mode::TELEOP:
			goals=teleop(in,main_joystick,gunner_joystick,panel,toplevel_status,level,low,top,cheval,portcullis);
			//test
			//tagThis("Line 347: switch(mode) teleop", __FILE__);
			//cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$tag!";
			//test			
			break;
		case Mode::AUTO_NULL:
			break;
		case Mode::AUTO_NAV:
			
			myfile2 << "AUTO_NAV:" << nav2.stepcounter << endl;

			//if(stepcounter==1) {
				nav2.NavV = nav2.loadnav(nav2.s1);
				myfile2 << "doing S1" << endl;
			//} else if(stepcounter==2) {
				//NavV = loadnav(s2);
				//myfile2 << "doing S2" << endl;
			//} else if(stepcounter==3){
				//Aturn.l = -.45;
				//Aturn.r = .45;
				//Aturn.dur=1;
			//	break;
			//}
			//else if(stepcounter==4)
				//fire the ball!
				//break;
			//else if(stepcounter==5)
				//all done
				//break;
			/*
			else if(stepcounter==5)
			
				break;
			else if(stepcounter==6)
				NavV = loadnav(s3);
			else if(stepcounter==7)
				NavV = loadnav(s4);
			*/
			//else
			//	assert(0);

			nav2.navindex = 0;
			myfile2 << "Nav loaded:" << nav2.NavV.size() << endl;
			myfile2.flush();
			break;

		case Mode::AUTO_NAV_RUN:
			goals.tilt=level;
			goals.drive.left=nav2.NavV[nav2.navindex].left;
			goals.drive.right=nav2.NavV[nav2.navindex].right;
			break;

		case Mode::AUTO_MOVE:
			goals.drive.left=nav2.Aturn.l;
			goals.drive.right=nav2.Aturn.r;
			break;
		case Mode::AUTO_SCORE:
			//score on low goal.
			break;
		case Mode::AUTO_REACH:
			myfile2 << "RUN:ON" << endl;
			goals.drive.left=-.45;
			goals.drive.right=-.45;
			break;
		case Mode::AUTO_STATIC:
			//goals.tilt=LEVEL;
			goals.drive.left=-1;
			goals.drive.right=-1;
			break;
		case Mode::AUTO_STATICTWO:
			//goals.tilt=LEVEL;
			goals.drive.left=-.5;
			goals.drive.right=-.5;
			break;
		case Mode::AUTO_STOP:
			myfile2 << "RUN:OFF" << endl;
			goals.drive.left=0;
			goals.drive.right=0;
			break;
		case Mode::AUTO_TEST:
			goals.drive.left=.10;
			goals.drive.right=.10;
		default: assert(0);
	}
	auto next=next_mode(mode,in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,nav2.navindex,nav2.NavV,nav2.stepcounter,nav2.Aturn);
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
#include "tilt.h"

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

Tilt_presets rand(Tilt_presets*){
	Tilt_presets r;
	#define X(NAME) r.NAME=rand()%10;
	PRESETS
	#undef X
	return r;
}

void test_preset_rw(){
	auto a=rand((Tilt_presets*)nullptr);
	write_tilt_presets(a);
	auto b=read_tilt_presets();
	assert(a==b);
}

int main(){
	Main m;
	Robot_inputs rin;
	m(rin);
	cout << "Test M: " << m << endl;
	//for tests


	m.mode=Main::Mode::TELEOP;
	for (unsigned i=0;i<1000;i++) {
		Robot_inputs in;
		in.now=i/100.0;
		in.robot_mode.autonomous=0;
		in.robot_mode.enabled=1;
		in.joystick[2].axis[2] = -1;
		in.joystick[2].button[1] = 1;
		in.joystick[2].button[2] = 1;
		in.joystick[2].button[3] = 1;
		in.analog[0] = degrees_to_volts(m.tilt_presets.top);
		if (i > 500) in.joystick[2].axis[2] = .62;
		if (i > 550) in.joystick[2].axis[2] = -1;
		if (i > 600) in.analog[0] = degrees_to_volts(m.tilt_presets.cheval);	
		stringstream ss;
		Robot_outputs out = m(in,ss);
		cout<<"Time: "<<in.now<<"    OI Button Axis: "<<in.joystick[2].axis[2]<<"   PWM: "<<out.pwm[4]<<"    Cheval Step: "<<static_cast<int>(m.cheval_step);
		cout<<"    Collector Mode: "<<m.collector_mode<<"    Left Wheels: "<<out.pwm[0]<<"    Right Wheels: "<<out.pwm[1]<<"\n";
	}

	test_preset_rw();
}

#endif
