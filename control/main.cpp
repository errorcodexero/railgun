#include "main.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <math.h>
#include <stdlib.h>
#include "toplevel.h"
#include "../util/util.h"
#include "../input/util.h"
#include "../util/point.h"
#include <vector>
#include <assert.h>
#include <fstream> 

using namespace std;

ofstream myfile2;

static int print_count=0;
#define SLOW_PRINT (print_count%10==0)

ostream& operator<<(ostream& o,Main::Mode a){
	#define X(NAME) if(a==Main::Mode::NAME) return o<<""#NAME;
	MODES
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Main::Shoot_steps a){
	#define X(NAME) if(a==Main::Shoot_steps::NAME) return o<<""#NAME;
	SHOOT_STEPS
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
	low=110;
	cheval=110;
}

#define PRESETS X(top) X(level) X(low) X(cheval)

bool operator==(Tilt_presets const& a,Tilt_presets const& b){
	#define X(NAME) if(a.NAME!=b.NAME) return 0;
	PRESETS
	#undef X
	return 1;
}

ostream& operator<<(ostream& o,Tilt_presets const& a){
	return o<<"Presets( top:"<<a.top<<"  level:"<<a.level<<"  low:"<<a.low<<"  cheval:"<<a.cheval<<")";
}

#ifdef MAIN_TEST
static const auto PRESET_FILE="presets1.txt";
static const string NAVLOG2="navlog2.txt";
#else
static const auto PRESET_FILE="/home/lvuser/presets1.txt";
static const string NAVLOG2="/home/lvuser/navlogs/navlog2.txt";
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
Main::Main():
	mode(Mode::TELEOP),
	autonomous_start(0),
	joy_collector_pos(Joy_collector_pos::STOP),
	collector_mode(Collector_mode::NOTHING),
	cheval_step(Cheval_steps::GO_DOWN),
	shoot_step(Shoot_steps::CLEAR_BALL)
{
	myfile2.open(NAVLOG2);
	myfile2 << "test start" << endl;
	tilt_presets=read_tilt_presets();
}


double set_drive_speed(double axis,double boost,double slow=0){
	static const float MAX_SPEED=1;//Change this value to change the max power the robot will achieve with full boost (cannot be larger than 1.0)
	static const float DEFAULT_SPEED=.4;//Change this value to change the default power
	static const float SLOW_BY=.5;//Change this value to change the percentage of the default power the slow button slows
	return (pow(axis,3)*((DEFAULT_SPEED+(MAX_SPEED-DEFAULT_SPEED)*boost)-((DEFAULT_SPEED*SLOW_BY)*slow)));
}

template<size_t LEN>
array<double,LEN> floats_to_doubles(array<float,LEN> a){
	array<double,LEN> r;
	for(size_t i=0;i<LEN;i++) r[i]=a[i];
	return r;
}

bool Main::get_learning()const{
	return learn.get() || !learn_delay.done();
}

double mean(double a,double b){
	return (a+b)/2;
}

array<double,3> mean(array<double,3> a,array<double,3> b){
	return array<double,3>{
		mean(a[0],b[0]),
		mean(a[1],b[1]),
		mean(a[2],b[2])
	};
}

Tilt::Goal mean(Tilt::Goal a,Tilt::Goal b){
	if(a.mode()==Tilt::Goal::Mode::GO_TO_ANGLE && b.mode()==Tilt::Goal::Mode::GO_TO_ANGLE){
		return Tilt::Goal::go_to_angle(mean(a.angle(),b.angle()));
	}else{
		return a;
	}
}

void Main::shooter_protocol(Shooter::Status_detail const& shooter_status, bool const& enabled, Time const& now, Toplevel::Goal& goals, bool shoot){
	const Tilt::Goal top=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));
	goals.collector.sides = Sides::Goal::OFF;
	goals.collector.tilt = top;
	static const Shooter::Goal::Type shoot_goal = Shooter::Goal::Type::GROUND_SHOT;
	switch(shoot_step){
		case Shoot_steps::CLEAR_BALL:
			if(false/*!shooter_status.beam*/) goals.collector.front = Front::Goal::CLEAR_BALL;
			else shoot_step = Shoot_steps::SPEED_UP;
			break;
		case Shoot_steps::SPEED_UP:
			goals.collector.front = Front::Goal::OFF;
			goals.shooter.type = shoot_goal;
			if(SLOW_PRINT) cout<<"\nshooter_status: "<<shooter_status<<" goals.shooter:"<<goals.shooter<<"\n";
			if(shoot && ready(shooter_status,goals.shooter)) shoot_step = Shoot_steps::SHOOT;
			break;
		case Shoot_steps::SHOOT:
			goals.collector.front = Front::Goal::IN;
			goals.shooter.type = shoot_goal;
			shoot_high_timer.update(now,enabled);
			if(shoot_high_timer.done()) collector_mode = Collector_mode::STOW; 
			break;
		default:
			assert(0);
	}
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
	Tilt::Goal drawbridge

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
		goals.drive.left=clip([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return -NUDGE_POWER;
			if(!nudges[Nudges::BACKWARD].timer.done()) return NUDGE_POWER;
			if(!nudges[Nudges::CLOCKWISE].timer.done()) return -NUDGE_CW_POWER;
			if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return -NUDGE_CCW_POWER;
			double power=set_drive_speed(main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power+=set_drive_speed(-main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return power;
		}());
		goals.drive.right=clip([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return -NUDGE_POWER;
			else if(!nudges[Nudges::BACKWARD].timer.done()) return NUDGE_POWER;
			else if(!nudges[Nudges::CLOCKWISE].timer.done()) return NUDGE_CW_POWER;
			else if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return NUDGE_CCW_POWER;
			double power=set_drive_speed(main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power+=set_drive_speed(main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return power;
		}());
	}
	
	bool ball=toplevel_status.collector.front.ball;
	
	controller_auto.update(gunner_joystick.button[Gamepad_button::START]);

	if(gunner_joystick.button[Gamepad_button::BACK] || main_joystick.button[Gamepad_button::BACK]){//Override sets all collector goals to off
		goals.collector={Front::Goal::OFF,Sides::Goal::OFF,Tilt::Goal::stop()};
		collector_mode=Collector_mode::NOTHING;
	}
	bool learning=get_learning();
	
	//if(SLOW_PRINT) cout<<tilt_presets<<"\n";
	
	if (panel.in_use) {//Collector modes and percentages
		if(panel.shooter_mode==Panel::Shooter_mode::CLOSED_AUTO) {
			goals.shooter.mode=Shooter::Goal::Mode::SPEED_AUTO;
			goals.shooter.percentage = 1.0;
		} else if(panel.shooter_mode==Panel::Shooter_mode::CLOSED_MANUAL) {
			goals.shooter.mode=Shooter::Goal::Mode::SPEED_MANUAL;
			goals.shooter.percentage = 1 + ((panel.speed_dial * 20) * .01);
		} else if(panel.shooter_mode==Panel::Shooter_mode::OPEN) {
			goals.shooter.mode=Shooter::Goal::Mode::VOLTAGE;
			goals.shooter.percentage = (panel.speed_dial + 1) / 2;
		}
	}
	
	if((!panel.in_use && controller_auto.get()) || (panel.in_use && (panel.tilt_auto || panel.front_auto || panel.sides_auto))) {//Automatic collector modes
		bool joy_learn=gunner_joystick.button[Gamepad_button::B];
		POV_section gunner_pov = pov_section(gunner_joystick.pov);
		if(gunner_joystick.button[Gamepad_button::X] || (panel.in_use && panel.shoot_low)){
			const Time SHOOT_TIME=1;			
			collector_mode=Collector_mode::SHOOT_LOW;
			shoot_low_timer.set(SHOOT_TIME);
		} else if((gunner_pov==POV_section::UP && !joy_learn) || (panel.in_use && panel.collector_pos==Panel::Collector_pos::STOW && !learning)) collector_mode = Collector_mode::STOW;
		else if((gunner_pov==POV_section::RIGHT && !joy_learn) || (panel.in_use && panel.cheval && !learning)) {
			collector_mode = Collector_mode::CHEVAL;
			cheval_lift_timer.set(.45);
			cheval_drive_timer.set(2);
			cheval_step = Cheval_steps::GO_DOWN;
		} else if (panel.in_use && panel.drawbridge && !learning) collector_mode=Collector_mode::DRAWBRIDGE;
		else if(gunner_joystick.button[Gamepad_button::Y] || (panel.in_use && panel.shoot_prep)){
			collector_mode=Collector_mode::SHOOT_HIGH;
			shoot_step = Shoot_steps::CLEAR_BALL;
			const Time SHOOT_TIME=1.5;
			shoot_high_timer.set(SHOOT_TIME);//time until we can assume the ball had been shot after being injected
		} else if((main_joystick.button[Gamepad_button::START] && !joy_learn) || (panel.in_use && panel.collect && !learning)) collector_mode=Collector_mode::COLLECT;
		else if(panel.in_use && panel.collector_pos==Panel::Collector_pos::LOW && !learning) collector_mode=Collector_mode::LOW;

		//if(SLOW_PRINT)cout<<"collector_mode: "<<collector_mode<<"\n";

		switch(collector_mode){
			case Collector_mode::COLLECT:
				goals.collector={Front::Goal::IN,Sides::Goal::IN,level};
				if(ball) collector_mode=Collector_mode::STOW;
				break;
			case Collector_mode::STOW:
				goals.collector={Front::Goal::OFF,Sides::Goal::OFF,top};
				break;
			case Collector_mode::REFLECT:
				goals.collector={Front::Goal::OFF,Sides::Goal::OUT,level};
				break;
			case Collector_mode::SHOOT_HIGH:
				shooter_protocol(toplevel_status.shooter,in.robot_mode.enabled,in.now,goals,panel.shoot_high);
				break;
			case Collector_mode::SHOOT_LOW:
				goals.collector={Front::Goal::OUT,Sides::Goal::OFF,top};
				shoot_low_timer.update(in.now, enabled);
				if (shoot_low_timer.done()) collector_mode = Collector_mode::STOW;
				break;
			case Collector_mode::LOW:
				goals.collector={Front::Goal::OFF,Sides::Goal::OFF,low};
				break;
			case Collector_mode::NOTHING:
				goals.collector={Front::Goal::OFF,Sides::Goal::OFF,Tilt::Goal::stop()};
				break;
			case Collector_mode::CHEVAL:
				{
					const double AUTO_POWER=-.5;
					goals.collector.front=Front::Goal::OFF;
					goals.collector.sides=Sides::Goal::OFF;
					switch(cheval_step) {
						case Cheval_steps::GO_DOWN: 
							goals.collector.tilt=cheval;
							if(ready(status(toplevel_status.collector.tilt),goals.collector.tilt)) cheval_step=Cheval_steps::DRIVE;
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
							goals.collector.tilt=top;
							cheval_drive_timer.update(in.now,enabled);
							if (cheval_drive_timer.done()) collector_mode=Collector_mode::STOW;
							break;
						default: 
							assert(0);
					}
					break;
				}
			case Collector_mode::DRAWBRIDGE:
				goals.collector={Front::Goal::OFF,Sides::Goal::OFF,drawbridge};
				break;
			default: assert(0);
		}
	}
	if (!panel.in_use && !controller_auto.get()) {//Manual joystick controls 
		goals.collector.front=[&]{
			const double LIMIT=.5; 
			if(gunner_joystick.axis[Gamepad_axis::LTRIGGER]>LIMIT) return Front::Goal::OUT;
			if(gunner_joystick.axis[Gamepad_axis::RTRIGGER]>LIMIT) return Front::Goal::IN;
			return Front::Goal::OFF;
		}();
		goals.collector.sides=[&]{
			if(gunner_joystick.button[Gamepad_button::RB]) return Sides::Goal::IN;
			if(gunner_joystick.button[Gamepad_button::LB]) return Sides::Goal::OUT;
			return Sides::Goal::OFF;
		}();
		goals.collector.tilt=[&]{
			{
				Joystick_section tilt_control = joystick_section(gunner_joystick.axis[Gamepad_axis::RIGHTX],gunner_joystick.axis[Gamepad_axis::RIGHTY]);
				#define X(NAME,SECTION) bool NAME=tilt_control==Joystick_section::SECTION;
				X(down_b,DOWN) X(up_b,UP) X(level_b,LEFT) X(low_b,RIGHT)
				#undef X
				if(low_b) joy_collector_pos = Joy_collector_pos::LOW;
				else if(level_b) joy_collector_pos = Joy_collector_pos::LEVEL;
				if(gunner_joystick.button[Gamepad_button::B]){//learn
					double learn_this=toplevel_status.collector.tilt.angle;
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
	}
	learn_delay.update(in.now,true);//update always because it's just a nice delay, not actually a key part of functionality
	if(!panel.in_use){
		if(gunner_joystick.button[Gamepad_button::R_JOY]) goals.shooter.mode=Shooter::Goal::Mode::SPEED_AUTO;
		else goals.shooter.mode=Shooter::Goal::Mode::VOLTAGE;
	}
	if (panel.in_use) {//Panel manual modes
		learn.update(panel.learn);
		if(learn.get()){//learn
			double learn_this=toplevel_status.collector.tilt.angle;
			bool done=false;
			if(panel.collect){
				tilt_presets.level=learn_this;
				done=true;
			}
			else if(panel.collector_pos==Panel::Collector_pos::STOW || panel.shoot_low){
				tilt_presets.top=learn_this;
				done=true;
			}
			else if(panel.cheval){
				tilt_presets.cheval=learn_this;
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
			#define X(name) if(panel.front==Panel::Collector::name) goals.collector.front = Front::Goal::name;
			X(IN) X(OUT) X(OFF)
			#undef X
		}
		if (!panel.sides_auto) {
			#define X(name) if(panel.sides==Panel::Collector::name) goals.collector.sides = Sides::Goal::name;
			X(IN) X(OFF) X(OUT)
			#undef X
		}
		if (!panel.tilt_auto) {
			goals.collector.tilt=[&]{
				if (panel.collector_up) return Tilt::Goal::up();
				if (panel.collector_down) return Tilt::Goal::down();
				return goals.collector.tilt=Tilt::Goal::stop();
			}();
		}
	}
	goals.climb_release=[&]{
		if(panel.in_use) {
			if(panel.lock_climber) return Climb_release::Goal::OUT;
			return Climb_release::Goal::IN;
		}
		return Climb_release::Goal::OUT;
	}();
	goals.winch=[&]{
		if(toplevel_status.climb_release==Climb_release::Status_detail::IN){
			if(!panel.in_use){
				switch (joystick_section(gunner_joystick.axis[Gamepad_axis::LEFTX], gunner_joystick.axis[Gamepad_axis::LEFTY])){
					case Joystick_section::UP: 
					case Joystick_section::DOWN: return Winch::Goal::IN;
					default: break;
				}
			} else {
				switch(panel.winch){
					case Panel::Winch::UP: 
					case Panel::Winch::DOWN: return Winch::Goal::IN;	
					case Panel::Winch::STOP: return Winch::Goal::STOP;
					default: assert(0);
				}
			}
		}
		return Winch::Goal::STOP;
	}();
	if(SLOW_PRINT) cout<<" "<<shoot_step<<"  "<<toplevel_status.shooter<<"   "<<goals.shooter<<"\n";
	return goals;
}

pair<float,float> driveatwall(const Robot_inputs in){
	const float targetinches=3; //Desired distance from wall
	float currentinches=range(in);
	pair<float,float> motorvoltmods;
	//motorvoltmods.first = left; motorvoltmods.second = right
	float adjustment=0;
	if(targetinches<currentinches){
		motorvoltmods.first=adjustment;
		motorvoltmods.second=adjustment*-1;
	}
	return motorvoltmods;
}

Main::Mode next_mode(Main::Mode m,bool autonomous,bool autonomous_start,Toplevel::Status_detail& /*status*/,Time since_switch, Panel panel,bool const&toplready,Robot_inputs const& in){
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
						case Panel::Auto_mode::PORTCULLIS:
							return Main::Mode::AUTO_PORTCULLIS;
						case Panel::Auto_mode::CHEVAL:
							return Main::Mode::AUTO_CHEVALPOS;
						default: assert(0);
					}
				}
				return Main::Mode::TELEOP; //during testing put the mode you want to test without the driverstation.
			}
			return m;

		case Main::Mode::AUTO_NULL:
			return Main::Mode::TELEOP;
		
		case Main::Mode::AUTO_REACH:
			myfile2 << "NEXT_MODE:SS" << since_switch << endl;
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch > .8) return Main::Mode::AUTO_STOP;
			return Main::Mode::AUTO_REACH;

		case Main::Mode::AUTO_STATIC:
			if (!autonomous) return Main::Mode::TELEOP;
			if(since_switch > 2) return Main::Mode::AUTO_STOP; //Time used to be 1.5, prior to match 65 PNW, where it was ~3in short on the rampart crossings
			return Main::Mode::AUTO_STATIC;

		case Main::Mode::AUTO_STATICTWO:
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch > 2.5) return Main::Mode::AUTO_STOP;
			return Main::Mode::AUTO_STATICTWO;

		case Main::Mode::AUTO_STOP:
			myfile2 << "NEXT_MODE:DONE=>TELEOP" << endl;
			return Main::Mode::TELEOP;

		case Main::Mode::AUTO_TEST:
			if(since_switch > 1 || !autonomous) return Main::Mode::TELEOP;
			return Main::Mode::AUTO_TEST;

		case Main::Mode::AUTO_PORTCULLIS:
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch > 2.5) return Main::Mode::AUTO_PORTCULLIS_DONE;
			return Main::Mode::AUTO_PORTCULLIS;
	
		case Main::Mode::AUTO_PORTCULLIS_DONE:
			if(since_switch > 2.5 || !autonomous) return Main::Mode::TELEOP;
			return Main::Mode::AUTO_PORTCULLIS_DONE;

		case Main::Mode::AUTO_CHEVALPOS:
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch > .75) return Main::Mode::AUTO_CHEVALDROP;
			return Main::Mode::AUTO_CHEVALPOS;

		case Main::Mode::AUTO_CHEVALDROP:
			if(!autonomous) return Main::Mode::TELEOP;
			if(toplready) return Main::Mode::AUTO_CHEVALDRIVE;
			return Main::Mode::AUTO_CHEVALDROP;
				
		case Main::Mode::AUTO_CHEVALDRIVE:
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch > .45) return Main::Mode::AUTO_CHEVALSTOW;
			return Main::Mode::AUTO_CHEVALDRIVE;
		
		case Main::Mode::AUTO_CHEVALSTOW:
			if(since_switch > 1.5 || !autonomous) return Main::Mode::TELEOP;
			return Main::Mode::AUTO_CHEVALSTOW;

		case Main::Mode::AUTO_SCORELOW:
			if(!autonomous) return Main::Mode::TELEOP;
			if(since_switch > 4.5) return Main::Mode::AUTO_SCOREBAR;
			return Main::Mode::AUTO_SCORELOW;
		case Main::Mode::AUTO_SCOREBAR:
			if(since_switch > 2 || !autonomous) return Main::Mode::TELEOP;
			return Main::Mode::AUTO_SCOREBAR;
		case Main::Mode::AUTO_SCOREDRIVE:
			{
				bool first_run=true;
				int ioencoder;
				int curencoder;

				first_run=false;
				curencoder = in.digital_io.encoder[0];

				if(!autonomous) return Main::Mode::TELEOP;

				if(first_run==false){
					ioencoder=0;
				} else {
					ioencoder = curencoder;
				}
			
				if((in.digital_io.encoder[0]) > ((curencoder - ioencoder) == 1000)) return Main::Mode::AUTO_SCOREPREP;

					return Main::Mode::AUTO_SCOREDRIVE;
			}
		/*default:
			if(!autonomous) return Main::Mode::TELEOP;
			return Main::Mode::AUTO_NULL;*/
		case Main::Mode::AUTO_SCOREPREP:
			if(!autonomous) return Main::Mode::TELEOP;
			return Main::Mode::AUTO_SCOREPREP;
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

	if(!in.robot_mode.enabled) collector_mode=Collector_mode::NOTHING;

	Tilt::Goal level=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.level));
	Tilt::Goal low=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.low));
	Tilt::Goal top=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));
	Tilt::Goal cheval=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.cheval));
	Tilt::Goal drawbridge=mean(top,level);
	
	if(!in.robot_mode.enabled){
		shoot_step = Main::Shoot_steps::CLEAR_BALL;
		collector_mode = Collector_mode::NOTHING;
		cheval_step = Cheval_steps::GO_DOWN;
	}

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
			goals=teleop(in,main_joystick,gunner_joystick,panel,toplevel_status,level,low,top,cheval,drawbridge);
			break;
		case Mode::AUTO_NULL:
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
			break;
		case Mode::AUTO_PORTCULLIS:
			goals.collector.front=Front::Goal::OFF;
			goals.collector.sides=Sides::Goal::OFF;
			goals.collector.tilt=low;

			if(ready(toplevel_status.collector.tilt.angle,goals.collector.tilt=low)){
				goals.drive.left=-.75;
				goals.drive.right=-.75;
			}
			break;
		case Mode::AUTO_PORTCULLIS_DONE:
			//goals.drive.left=0;
			//goals.drive.right=0;
			goals.collector.tilt=top;
			break;

		case Mode::AUTO_CHEVALPOS:
			goals.collector.front=Front::Goal::OFF;
			goals.collector.sides=Sides::Goal::OFF;
			goals.drive.left=-.45;
			goals.drive.right=-.45;
			break;
		
		case Mode::AUTO_CHEVALDROP:
			goals.drive.left=0;
			goals.drive.right=0;
			goals.collector.front=Front::Goal::OFF;
			goals.collector.sides=Sides::Goal::OFF;
			goals.collector.tilt=cheval;
			Main::topready=ready(toplevel_status.collector.tilt.angle,goals.collector.tilt);
			
			break;

		case Mode::AUTO_CHEVALDRIVE:
			goals.collector.front=Front::Goal::OFF;
			goals.collector.sides=Sides::Goal::OFF;
			goals.drive.right=-.5;
			goals.drive.left=-.5;
			break;
			
		case Mode::AUTO_CHEVALSTOW:
			goals.collector.front=Front::Goal::OFF;
			goals.collector.sides=Sides::Goal::OFF;
			goals.drive.right=-.5;
			goals.drive.left=-.5;
			goals.collector.tilt=top;
			break;
		case Mode::AUTO_SCORELOW:
			break;
		case Mode::AUTO_SCOREBAR:
			break;
		case Mode::AUTO_SCOREDRIVE:
			break;
		case Mode::AUTO_SCOREPREP:	
			break;

		//shooter_protical call in here takes in robot inputs,toplevel goal,toplevel status detail
		default: assert(0);
	}
	auto next=next_mode(mode,in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,topready,in);
	since_switch.update(in.now,mode!=next);
	mode=next;
		
	Toplevel::Output r_out=control(toplevel_status,goals); 
	auto r=toplevel.output_applicator(Robot_outputs{},r_out);
	
	r=force(r);
	auto input=toplevel.input_reader(in);

	r.panel_output[Panel_outputs::SPUN_UP] = Panel_output(static_cast<int>(Panel_output_ports::SPUN_UP), (collector_mode==Collector_mode::SHOOT_HIGH && ready(toplevel_status.shooter,goals.shooter))); 

	/*auto talonPower = Talon_srx_output();
	talonPower.power_level = .5;
	r.talon_srx[0]= talonPower;*/
	
	toplevel.estimator.update(
		in.now,
		input,
		toplevel.output_applicator(r)
	);
	log(in,toplevel_status,r);
	return r;
}

bool operator==(Main const& a,Main const& b){
	return a.force==b.force && 
		a.perf==b.perf && 
		a.toplevel==b.toplevel && 
		a.since_switch==b.since_switch && 
		a.since_auto_start==b.since_auto_start &&
		a.autonomous_start==b.autonomous_start;
}

bool operator!=(Main const& a,Main const& b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Main const& m){
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

void test_teleop(){
	Main m;
	m.mode=Main::Mode::TELEOP;
	
	Robot_inputs in;
	in.now=0;
	in.robot_mode.autonomous=0;
	in.robot_mode.enabled=1;//enable robot
	in.joystick[2].button[1] = 1;//set tilt to auto
	in.joystick[2].button[2] = 1;//set sides to auto
	in.joystick[2].button[3] = 1;//set front to auto
	in.joystick[2].axis[2] = -1;//set all buttons to off
	in.analog[0] = degrees_to_volts(m.tilt_presets.top);//start at top

	const Time RUN_TIME=5;//in seconds
	int print_count=0;
	
	const Time INCREMENT=.01;
	
	while(in.now<=RUN_TIME){	
		static const Time PUSH_CHEVAL=1,RELEASE_CHEVAL=1.5,ARRIVE_AT_CHEVAL_GOAL=3;
		
		if(in.now >= PUSH_CHEVAL) in.joystick[2].axis[2] = .62;
		if(in.now >= RELEASE_CHEVAL) in.joystick[2].axis[2] = -1;
		if(in.now >= ARRIVE_AT_CHEVAL_GOAL) in.analog[0] = degrees_to_volts(m.tilt_presets.cheval);	
		
		stringstream ss;
		Robot_outputs out = m(in,ss);
		//Panel panel=interpret(in.joystick[2]);
		
		if(SLOW_PRINT){
			cout<<"Now: "<<in.now<<"    Panel buttons: "<<in.joystick[2].axis[2]<<"   PWM: "<<out.pwm[4]<<"    Collector Mode: "<<m.collector_mode;
			if(m.collector_mode==Main::Collector_mode::CHEVAL) cout<<"    Cheval Step: "<<m.cheval_step;
			cout<<"    Left Wheels: "<<out.pwm[0]<<"    Right Wheels: "<<out.pwm[1]<<"\n";
		}
		
		in.now+=INCREMENT;
		print_count++;
	}
}

void update_pos(Pt &current_pos, Robot_outputs out, const Time INCREMENT){
	const double FT_PER_SEC = 10;//ft/sec assumed for full power for now and that different percent powers correspond to the same percent of that assumption
	const double RAD_PER_SEC = 1.96;//rad/sec assumed for now at full speed
	double x_diff = 0, y_diff = 0, theta_diff = 0;
	if (-out.pwm[0] == out.pwm[1]) {
		double dist = FT_PER_SEC * INCREMENT;
		x_diff = cos(current_pos.theta) * (dist * out.pwm[0]);
		y_diff = sin(current_pos.theta) * (dist * out.pwm[0]);
		if (fabs(x_diff) < .000001) x_diff = 0;
		if (fabs(y_diff) < .000001) y_diff = 0;
		//cout<<"\nx:"<<cos(current_pos.theta)<<"   y:"<<sin(current_pos.theta)<<"    x:"<<x_diff<<"    y:"<<y_diff<<"    t:"<<theta_diff<<"\n";
	} else {
		theta_diff = RAD_PER_SEC * INCREMENT * out.pwm[0];//assuming robot is either driving straight or turning in place
	}
	current_pos+={x_diff,y_diff,theta_diff};
}

void test_autonomous(Main::Mode mode){
	Main m;

	m.mode=mode;
	
	Robot_inputs in;
	in.now=0;
	in.robot_mode.autonomous=1;
	in.robot_mode.enabled=1;
	
	const Time RUN_TIME=4;//in seconds
	Pt pos;//0rad is right
	pos.theta=PI/2;//start facing forward
	
	const Time INCREMENT=.01;	
	int print_count=0;
	
	while(in.now<=RUN_TIME){
		stringstream ss;
		Robot_outputs out=m(in,ss);
		
		update_pos(pos,out,INCREMENT);
		
		if(SLOW_PRINT) cout<<"Now: "<<in.now<<"    Left wheels: "<<out.pwm[0]<<"     Right wheels: "<<out.pwm[1]<<"   Position: "<<pos<<"\n";
		
		in.now+=INCREMENT;
		print_count++;
	}
}

void test_modes(){
	const vector<Main::Mode> MODE_LIST{
		#define X(NAME) Main::Mode::NAME,
		MODES
		#undef X
	};
	for(Main::Mode mode:MODE_LIST){
		cout<<"\nTest mode: "<<mode<<"\n\n"; 
		if(mode==Main::Mode::TELEOP) test_teleop();
		else test_autonomous(mode);
	}
}

void test_next_mode(){
	//Main::Mode next_mode(Main::Mode m,bool autonomous,bool autonomous_start,Toplevel::Status_detail /*status*/,Time since_switch, Panel panel,unsigned int navindex,vector<Nav2::NavS> NavV,int & stepcounter,Nav2::aturn Aturn){
	vector<Main::Mode> MODE_LIST{
		#define X(NAME) Main::Mode::NAME,
		MODES
		#undef X
	};
	for(auto mode:MODE_LIST){
		Toplevel::Status_detail st=example((Toplevel::Status_detail*)nullptr);
		bool toplready = true;
		Robot_inputs in;
		
		
		auto next=next_mode(mode,0,0,st,0,Panel{},toplready,in);
		cout<<"Testing mode "<<mode<<" goes to "<<next<<"\n";
		assert(next==Main::Mode::TELEOP);
	}
}

int main(){
	test_next_mode();
	test_modes();
	test_preset_rw();
}

#endif
