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
#include "../executive/teleop.h"

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

#ifdef MAIN_TEST
static const string NAVLOG2="navlog2.txt";
#else
static const string NAVLOG2="/home/lvuser/navlogs/navlog2.txt";
#endif

//TODO: at some point, might want to make this whatever is right to start autonomous mode.
Main::Main():
	mode(Mode::TELEOP),
	mode_(Executive{Teleop()}),
	motion_profile(0.0,.01),
	in_br_range(),
	autonomous_start(0),
	joy_collector_pos(Joy_collector_pos::STOP),
	collector_mode(Collector_mode::NOTHING),
	tilt_learn_mode(0),
	cheval_step(Cheval_steps::GO_DOWN),
	shoot_step(Shoot_steps::SPEED_UP)
{
	in_br_range.set(2.0);
	set_initial_encoders = true;
	initial_encoders = make_pair(0,0);
	br_step=0;
	myfile2.open(NAVLOG2);
	myfile2 << "test start" << endl;
	tilt_presets=read_tilt_presets();
	shooter_constants=read_shooter_constants();
}


template<size_t LEN>
array<double,LEN> floats_to_doubles(array<float,LEN> a){
	array<double,LEN> r;
	for(size_t i=0;i<LEN;i++) r[i]=a[i];
	return r;
}

Shooter::Goal Main::shoot_action(Panel::Shooter_mode shooter_mode,double speed_dial,bool climbed_shot)const{
	switch(shooter_mode){
		case Panel::Shooter_mode::CLOSED_AUTO:
			return Shooter::Goal{
				shooter_constants.pid,
				Shooter::Goal::Mode::SPEED,
				climbed_shot?shooter_constants.climbed:shooter_constants.ground
			};
		case Panel::Shooter_mode::CLOSED_MANUAL:
			return Shooter::Goal{
				shooter_constants.pid,
				Shooter::Goal::Mode::SPEED,
				(climbed_shot?shooter_constants.ground:shooter_constants.climbed) * ( 1 + ((speed_dial * 20) * .01))
			};
		case Panel::Shooter_mode::OPEN:
			return Shooter::Goal{
				shooter_constants.pid,
				Shooter::Goal::Mode::VOLTAGE,
				-(speed_dial+1)/2
			};
		default:
			assert(0);
	}
}

void Main::shooter_protocol(
	Toplevel::Status_detail const& status,
	const bool enabled,
	const Time now,
	Toplevel::Goal& goals,
	bool shoot,
	Panel::Shooter_mode shooter_mode,
	double speed_dial
){
	const Tilt::Goal top=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));
	goals.collector.sides = Sides::Goal::OFF;
	goals.collector.tilt = top;
	switch(shoot_step){
		case Shoot_steps::SPEED_UP:
			goals.collector.front = Front::Goal::OFF;
			goals.shooter=shoot_action(shooter_mode,speed_dial,status.winch.partly_climbed);
			if(shoot && ready(status.shooter,goals.shooter)) shoot_step = Shoot_steps::SHOOT;
			break;
		case Shoot_steps::SHOOT:
			goals.collector.front = Front::Goal::IN;
			goals.shooter=shoot_action(shooter_mode,speed_dial,status.winch.partly_climbed);
			shoot_high_timer.update(now,enabled);
			if(shoot_high_timer.done()) collector_mode = Collector_mode::STOW;
			break;
		default:
			assert(0);
	}
}

void Main::cal(Time now,double current_tilt_angle,double current_shooter_speed,Panel const& panel){
	bool set=set_button(now,panel.in_use && panel.learn);

	if(tilt_learn_mode){
		auto update=[&](double &d){
			d=current_tilt_angle;
			write_tilt_presets(tilt_presets);
			tilt_learn_mode=0;
		};

		if(panel.collect){
			update(tilt_presets.level);
		}else if(panel.collector_pos==Panel::Collector_pos::STOW || panel.shoot_low){
			update(tilt_presets.top);
		}else if(panel.cheval){
			update(tilt_presets.cheval);
		}else if(panel.collector_pos==Panel::Collector_pos::LOW){
			update(tilt_presets.low);
		}else if(set){
			tilt_learn_mode=0;
		}
		return;
}
	if(!set) return;

	auto show=[&](){
		//cout<<"New shooter constants:"<<shooter_constants<<"\n";
	};
	auto adjust=[&](float &f){
		//assuming that speed dial comes in with a range of -1 to 1
		if(fabs(f)<.0001 && fabs(panel.speed_dial)>.75) f=(panel.speed_dial>0 ? .01 : -.01);
		else f*=panel.speed_dial*2;
		write_shooter_constants(shooter_constants);
		show();
	};
	switch(panel.auto_switch){
		case 0:
			tilt_learn_mode=1;
			return;
		#define SHOOTER_ASSIGNMENTS \
			X(1,pid.p)\
			X(2,pid.i)\
			X(3,pid.d)\
			X(4,pid.f)\
			X(5,ground)\
			X(6,climbed)
		#define X(A,B) case A: adjust(shooter_constants.B); return;
		SHOOTER_ASSIGNMENTS
		#undef X
		case 7:
			//reset all the values to defaults
			shooter_constants=Shooter_constants();
			write_shooter_constants(shooter_constants);
			show();
			return;
		#define SHOOTER_SPEEDS \
			X(8,ground)\
			X(9,climbed)
		#define X(A,B) case A: shooter_constants.B=current_shooter_speed; write_shooter_constants(shooter_constants); return;
		SHOOTER_SPEEDS
		#undef X
		/*case 8:
			//Set ground shot speed based on current
			shooter_constants.ground=current_shooter_speed;
			write_shooter_constants(shooter_constants);
			return;
		case 9:
			//Set climbed shot speed based on current
			return;*/
		default:
			return;
	}
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

/*double ticks_to_inches(int ticks){//Moved to drivebase.cpp
	const unsigned int TICKS_PER_REVOLUTION=100;
	const double WHEEL_DIAMETER=8.0;//inches
	const double WHEEL_CIRCUMFERENCE=WHEEL_DIAMETER*PI;//inches
	const double INCHES_PER_TICK=WHEEL_CIRCUMFERENCE/(double)TICKS_PER_REVOLUTION;//0.25 vs 0.251327
	return ticks*INCHES_PER_TICK;
}*/

double ticks_to_degrees(int ticks){
	const double DEGREES_PER_TICK=0.716197244;//Assumed for now
	return ticks * DEGREES_PER_TICK;//Degrees clockwise
}

Main::Mode get_auto(Panel const& panel){
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
			case Panel::Auto_mode::LBLS:
				return Main::Mode::AUTO_LBLS_CROSS_LB;
			case Panel::Auto_mode::LBWLS:	
				return Main::Mode::AUTO_LBWLS_WALL;
			case Panel::Auto_mode::LBWHS:
				return Main::Mode::AUTO_LBWHS_WALL;
			case Panel::Auto_mode::S:
				return Main::Mode::AUTO_LBWHS_PREP;
			case Panel::Auto_mode::BR:
				return Main::Mode::AUTO_BR_STRAIGHTAWAY;
			default: assert(0);
		}
	}
	return Main::Mode::TELEOP;
}

Robot_outputs Main::operator()(Robot_inputs in,ostream&){
	print_count++;

	perf.update(in.now);
	Joystick_data main_joystick=in.joystick[0];
	Joystick_data gunner_joystick=in.joystick[1];
	Panel panel=interpret(in.joystick[2]);

	if(!in.robot_mode.enabled){
		shoot_step = Main::Shoot_steps::SPEED_UP;
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
		
	if(in.ds_info.connected && SLOW_PRINT) cout<<"panel:"<<panel<<"\n";
	
	bool autonomous_start_now=autonomous_start(in.robot_mode.autonomous && in.robot_mode.enabled);
	since_auto_start.update(in.now,autonomous_start_now);
		
	Toplevel::Goal goals;
	//decltype(in.current) robotcurrent;
	//for(auto &a:robotcurrent) a = 0;
	if((toplevel_status.drive.ticks.first && initial_encoders.first==10000) || (toplevel_status.drive.ticks.second && initial_encoders.second==10000)) set_initial_encoders=true;
	if(set_initial_encoders){
		set_initial_encoders=false;
		initial_encoders = toplevel_status.drive.ticks;	
	}
	goals = mode_.run(Run_info{in,main_joystick,gunner_joystick,panel,toplevel_status});
	auto next=mode_.next_mode(Next_mode_info{in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,in});
	if(in.ds_info.connected && SLOW_PRINT) cout<<"mode_: "<<mode_<<"\n";
	//next_mode(mode,in.robot_mode.autonomous,autonomous_start_now,toplevel_status,since_switch.elapsed(),panel,topready,in,initial_encoders,br_step,set_initial_encoders,motion_profile,in_br_range);
	
	since_switch.update(in.now,mode_/*mode*/!=next);
	mode_=next;
		
	Toplevel::Output r_out=control(toplevel_status,goals); 
	auto r=toplevel.output_applicator(Robot_outputs{},r_out);
	
	r=force(r);
	auto input=toplevel.input_reader(in);

	r.panel_output[Panel_outputs::SPUN_UP] = Panel_output(
		static_cast<int>(Panel_output_ports::SPUN_UP),
		collector_mode==Collector_mode::SHOOT_HIGH && ready(toplevel_status.shooter,goals.shooter)
	);

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
	o<<" "<<m.mode;
	o<<" "<<m.force;
	o<<" "<<m.perf;
	o<<" "<<m.toplevel;
	o<<" "<<m.since_switch;
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
	/*vector<Main::Mode> MODE_LIST{
		#define X(NAME) Main::Mode::NAME,
		MODES
		#undef X
	};
	for(auto mode:MODE_LIST){
		Toplevel::Status_detail st=example((Toplevel::Status_detail*)nullptr);
		bool topready = true;
		Robot_inputs in;
		unsigned int br_step=0;
		bool set_initial_encoders=true;	
		Motion_profile motion_profile;
		Countdown_timer in_br_range;
		auto next=next_mode(mode,0,0,st,0,Panel{},topready,in,make_pair(0,0),br_step,set_initial_encoders,motion_profile,in_br_range);
		cout<<"Testing mode "<<mode<<" goes to "<<next<<"\n";
		assert(next==Main::Mode::TELEOP);
	}*/
}

int main(){
	//test_next_mode();
	test_modes();
}

#endif
