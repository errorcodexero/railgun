#include "teleop.h"
#include <math.h>
#include "delay.h"
#include "../input/util.h"

using namespace std;

ostream& operator<<(ostream& o,Teleop::Shoot_steps a){
	#define X(NAME) if(a==Teleop::Shoot_steps::NAME) return o<<""#NAME;
	SHOOT_STEPS
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Teleop::Collector_mode a){
	#define X(NAME) if(a==Teleop::Collector_mode::NAME) return o<<""#NAME;
	COLLECTOR_MODES
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Teleop::Nudge const& ){
	o<<"Nudge(";
	o<<"...";
	return o<<")";
}

ostream& operator<<(ostream& o,Teleop::Cheval_steps a){
	#define X(NAME) if(a==Teleop::Cheval_steps::NAME) return o<<""#NAME;
	CHEVAL_STEPS
	#undef X
	assert(0);
}

ostream& operator<<(ostream& o,Teleop::Joy_collector_pos a){
	#define X(NAME) if(a==Teleop::Joy_collector_pos::NAME) return o<<""#NAME;
	JOY_COLLECTOR_POS
	#undef X
	assert(0);
}

double set_drive_speed(double axis,double boost,double slow){
	static const float MAX_SPEED=1;//Change this value to change the max power the robot will achieve with full boost (cannot be larger than 1.0)
	static const float DEFAULT_SPEED=.4;//Change this value to change the default power
	static const float SLOW_BY=.5;//Change this value to change the percentage of the default power the slow button slows
	return (pow(axis,3)*((DEFAULT_SPEED+(MAX_SPEED-DEFAULT_SPEED)*boost)-((DEFAULT_SPEED*SLOW_BY)*slow)));
}

bool operator<(Teleop::Nudge const& a,Teleop::Nudge const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	NUDGE_ITEMS(X)
	#undef X
	return 0;
}

bool operator==(Teleop::Nudge const& a,Teleop::Nudge const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	NUDGE_ITEMS(X)
	#undef X
	return 1;
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

Executive Teleop::next_mode(Next_mode_info info) {
	if (info.autonomous_start) {
		if (info.panel.in_use) {
			return Executive{Delay()};
		}
	}
	Teleop t(CONSTRUCT_STRUCT_PARAMS(TELEOP_ITEMS));
	return Executive{t};
}

Teleop::Teleop():
	shoot_step(Shoot_steps::SPEED_UP),
	collector_mode(Collector_mode::NOTHING),
	tilt_learn_mode(0),
	cheval_step(Cheval_steps::GO_DOWN),
	joy_collector_pos(Joy_collector_pos::STOP)
{
	/*//FIXME uninitialized, does it matter?
	shoot_high_timer;
	shoot_low_timer;
	cheval_lift_timer;
	cheval_drive_timer;
	nudges;
	controller_auto;
	cheval_trigger;
	set_button;
	*/

	tilt_presets=read_tilt_presets();
	shooter_constants=read_shooter_constants();
}

IMPL_STRUCT(Teleop::Teleop,TELEOP_ITEMS)

Shooter::Goal Teleop::shoot_action(Panel::Shooter_mode shooter_mode,double speed_dial,bool climbed_shot)const{
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

void Teleop::shooter_protocol(
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

/*double set_drive_speed(double axis,double boost,double slow=0){
	static const float MAX_SPEED=1;//Change this value to change the max power the robot will achieve with full boost (cannot be larger than 1.0)
	static const float DEFAULT_SPEED=.4;//Change this value to change the default power
	static const float SLOW_BY=.5;//Change this value to change the percentage of the default power the slow button slows
	return (pow(axis,3)*((DEFAULT_SPEED+(MAX_SPEED-DEFAULT_SPEED)*boost)-((DEFAULT_SPEED*SLOW_BY)*slow)));
}*/

void Teleop::cal(Time now,double current_tilt_angle,double current_shooter_speed,Panel const& panel){
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

Toplevel::Goal Teleop::run(Run_info info) {
	Toplevel::Goal goals;
	
	bool enabled = info.in.robot_mode.enabled;

	Tilt::Goal level=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.level));
	Tilt::Goal low=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.low));
	Tilt::Goal top=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.top));
	Tilt::Goal cheval=Tilt::Goal::go_to_angle(make_tolerances(tilt_presets.cheval));
	Tilt::Goal drawbridge=mean(top,level);	

	{//Set drive goals
		bool spin=fabs(info.main_joystick.axis[Gamepad_axis::RIGHTX])>.01;//drive turning button
		double boost=info.main_joystick.axis[Gamepad_axis::LTRIGGER],slow=info.main_joystick.axis[Gamepad_axis::RTRIGGER];//turbo and slow buttons	
	
		for(int i=0;i<NUDGES;i++){
			const array<unsigned int,NUDGES> nudge_buttons={Gamepad_button::Y,Gamepad_button::A,Gamepad_button::B,Gamepad_button::X};//Forward, backward, clockwise, counter-clockwise
			if(nudges[i].trigger(boost<.25 && info.main_joystick.button[nudge_buttons[i]])) nudges[i].timer.set(.1);
			nudges[i].timer.update(info.in.now,enabled);
		}
		const double NUDGE_POWER=.4,NUDGE_CW_POWER=.4,NUDGE_CCW_POWER=-.4; 
		goals.drive.left=clip([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return -NUDGE_POWER;
			if(!nudges[Nudges::BACKWARD].timer.done()) return NUDGE_POWER;
			if(!nudges[Nudges::CLOCKWISE].timer.done()) return -NUDGE_CW_POWER;
			if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return -NUDGE_CCW_POWER;
			double power=set_drive_speed(info.main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power+=set_drive_speed(-info.main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return power;
		}());
		goals.drive.right=clip([&]{
			if(!nudges[Nudges::FORWARD].timer.done()) return -NUDGE_POWER;
			else if(!nudges[Nudges::BACKWARD].timer.done()) return NUDGE_POWER;
			else if(!nudges[Nudges::CLOCKWISE].timer.done()) return NUDGE_CW_POWER;
			else if(!nudges[Nudges::COUNTERCLOCKWISE].timer.done()) return NUDGE_CCW_POWER;
			double power=set_drive_speed(info.main_joystick.axis[Gamepad_axis::LEFTY],boost,slow);
			if(spin) power+=set_drive_speed(info.main_joystick.axis[Gamepad_axis::RIGHTX],boost,slow);
			return power;
		}());
	}
	
	controller_auto.update(info.gunner_joystick.button[Gamepad_button::START]);

	if(info.gunner_joystick.button[Gamepad_button::BACK] || info.main_joystick.button[Gamepad_button::BACK]){//Override sets all collector goals to off
		goals.collector={Front::Goal::OFF,Sides::Goal::OFF,Tilt::Goal::stop()};
		collector_mode=Collector_mode::NOTHING;
	}
	//if(SLOW_PRINT) cout<<tilt_presets<<"\n";
	
	//goals.shooter.value=0;
	goals.shooter.constants=shooter_constants.pid;
	
	bool enter_cheval=cheval_trigger(info.in.now,info.panel.in_use&&info.panel.cheval)&&!tilt_learn_mode;
		
	if((!info.panel.in_use && controller_auto.get()) || (info.panel.in_use && (info.panel.tilt_auto || info.panel.front_auto || info.panel.sides_auto))) {//Automatic collector modes
		bool joy_learn=info.gunner_joystick.button[Gamepad_button::B];
		POV_section gunner_pov = pov_section(info.gunner_joystick.pov);
		if(info.gunner_joystick.button[Gamepad_button::X] || (info.panel.in_use && info.panel.shoot_low)){
			const Time SHOOT_TIME=1;			
			collector_mode=Collector_mode::SHOOT_LOW;
			shoot_low_timer.set(SHOOT_TIME);
		}else if(
			(gunner_pov==POV_section::UP && !joy_learn) 
			|| (info.panel.in_use && info.panel.collector_pos==Panel::Collector_pos::STOW && !tilt_learn_mode)
		){
			collector_mode = Collector_mode::STOW;
		} else if((gunner_pov==POV_section::RIGHT && !joy_learn) || enter_cheval) {
			collector_mode = Collector_mode::CHEVAL;
			cheval_lift_timer.set(.45);
			cheval_drive_timer.set(2);
			cheval_step = Cheval_steps::GO_DOWN;
		} else if (info.panel.in_use && info.panel.drawbridge && !tilt_learn_mode) collector_mode=Collector_mode::DRAWBRIDGE;
		else if(info.gunner_joystick.button[Gamepad_button::A] || (info.panel.in_use && info.panel.shoot_prep)){
			collector_mode=Collector_mode::SHOOT_HIGH;
			shoot_step = Shoot_steps::SPEED_UP;
			const Time SHOOT_TIME=1.5;
			shoot_high_timer.set(SHOOT_TIME);//time until we can assume the ball had been shot after being injected
		} else if((info.main_joystick.button[Gamepad_button::START] && !joy_learn) || (info.panel.in_use && info.panel.collect && !tilt_learn_mode)){
			 collector_mode=Collector_mode::COLLECT;
		}else if(info.panel.in_use && info.panel.collector_pos==Panel::Collector_pos::LOW && !tilt_learn_mode) collector_mode=Collector_mode::LOW;

		//if(SLOW_PRINT)cout<<"collector_mode: "<<collector_mode<<"\n";

		switch(collector_mode){
			case Collector_mode::COLLECT:
			{
				goals.collector={Front::Goal::IN,Sides::Goal::IN,level};
				bool ball=info.toplevel_status.collector.front.ball;
				if(ball) collector_mode=Collector_mode::STOW;
				break;
			}
			case Collector_mode::STOW:
				goals.collector={Front::Goal::OFF,Sides::Goal::OFF,top};
				break;
			case Collector_mode::REFLECT:
				goals.collector={Front::Goal::OFF,Sides::Goal::OUT,level};
				break;
			case Collector_mode::SHOOT_HIGH:
			{
				bool shoot_button=info.panel.shoot_high||info.gunner_joystick.button[Gamepad_button::Y];
				shooter_protocol(info.toplevel_status,info.in.robot_mode.enabled,info.in.now,goals,shoot_button,info.panel.shooter_mode,info.panel.speed_dial);
				break;
			}
			case Collector_mode::SHOOT_LOW:
				goals.collector={Front::Goal::OUT,Sides::Goal::OFF,top};
				shoot_low_timer.update(info.in.now, enabled);
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
							if(ready(status(info.toplevel_status.collector.tilt),goals.collector.tilt)) cheval_step=Cheval_steps::DRIVE;
							break;
						case Cheval_steps::DRIVE:
							goals.drive.right=AUTO_POWER;
							goals.drive.left=AUTO_POWER;
							cheval_lift_timer.update(info.in.now,enabled);
							if (cheval_lift_timer.done()) cheval_step=Cheval_steps::DRIVE_AND_STOW;
							break;
						case Cheval_steps::DRIVE_AND_STOW:
							goals.drive.right=AUTO_POWER;
							goals.drive.left=AUTO_POWER;
							goals.collector.tilt=top;
							cheval_drive_timer.update(info.in.now,enabled);
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
	if (!info.panel.in_use && !controller_auto.get()) {//Manual joystick controls 
		goals.collector.front=[&]{
			const double LIMIT=.5; 
			if(info.gunner_joystick.axis[Gamepad_axis::LTRIGGER]>LIMIT) return Front::Goal::OUT;
			if(info.gunner_joystick.axis[Gamepad_axis::RTRIGGER]>LIMIT) return Front::Goal::IN;
			return Front::Goal::OFF;
		}();
		goals.collector.sides=[&]{
			if(info.gunner_joystick.button[Gamepad_button::RB]) return Sides::Goal::IN;
			if(info.gunner_joystick.button[Gamepad_button::LB]) return Sides::Goal::OUT;
			return Sides::Goal::OFF;
		}();
		goals.collector.tilt=[&]{
			{
				Joystick_section tilt_control = joystick_section(info.gunner_joystick.axis[Gamepad_axis::RIGHTX],info.gunner_joystick.axis[Gamepad_axis::RIGHTY]);
				#define X(NAME,SECTION) bool NAME=tilt_control==Joystick_section::SECTION;
				X(down_b,DOWN) X(up_b,UP) X(level_b,LEFT) X(low_b,RIGHT)
				#undef X
				if(low_b) joy_collector_pos = Joy_collector_pos::LOW;
				else if(level_b) joy_collector_pos = Joy_collector_pos::LEVEL;
				if(info.gunner_joystick.button[Gamepad_button::B]){//learn
					double learn_this=info.toplevel_status.collector.tilt.angle;
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
	if(!info.panel.in_use){
		if(info.gunner_joystick.button[Gamepad_button::R_JOY]) goals.shooter.mode=Shooter::Goal::Mode::SPEED;
		else goals.shooter.mode=Shooter::Goal::Mode::VOLTAGE;
	}

	cal(info.in.now,info.toplevel_status.collector.tilt.angle,info.in.talon_srx[0].velocity,info.panel);
	
	if (info.panel.in_use) {//Panel manual modes
		if (!info.panel.front_auto) {
			#define X(name) if(info.panel.front==Panel::Collector::name) goals.collector.front = Front::Goal::name;
			X(IN) X(OUT) X(OFF)
			#undef X
		}
		if (!info.panel.sides_auto) {
			#define X(name) if(info.panel.sides==Panel::Collector::name) goals.collector.sides = Sides::Goal::name;
			X(IN) X(OFF) X(OUT)
			#undef X
		}
		if (!info.panel.tilt_auto) {
			goals.collector.tilt=[&]{
				if (info.panel.collector_up) return Tilt::Goal::up();
				if (info.panel.collector_down) return Tilt::Goal::down();
				return Tilt::Goal::stop();
			}();
		}
	}
	goals.climb_release=[&]{
		if(info.panel.in_use) {
			if(info.panel.lock_climber) return Climb_release::Goal::OUT;
			return Climb_release::Goal::IN;
		}
		return Climb_release::Goal::OUT;
	}();
	goals.winch=[&]{
		if(info.toplevel_status.climb_release==Climb_release::Status_detail::IN){
			if(!info.panel.in_use){
				switch (joystick_section(info.gunner_joystick.axis[Gamepad_axis::LEFTX], info.gunner_joystick.axis[Gamepad_axis::LEFTY])){
					case Joystick_section::UP: 
					case Joystick_section::DOWN: return Winch::Goal::IN;
					default: break;
				}
			} else {
				switch(info.panel.winch){
					case Panel::Winch::UP: 
					case Panel::Winch::DOWN: return Winch::Goal::IN;	
					case Panel::Winch::STOP: return Winch::Goal::STOP;
					default: assert(0);
				}
			}
		}
		return Winch::Goal::STOP;
	}();
	//if(SLOW_PRINT) cout<<shoot_step<<" "<<info.toplevel_status.shooter<<" "<<goals.shooter<<"\n";
	//if(SLOW_PRINT) cout<<info.toplevel_status.drive.speeds<<"\n";
	return goals;
}

#define TELEOP_ITEMS_NO_TYPE(X)\
	X(shooter_constants)\
	X(tilt_presets)\
	X(shoot_step)\
	X(shoot_high_timer)\
	X(shoot_low_timer)\
	X(cheval_lift_timer)\
	X(cheval_drive_timer)\
	X(collector_mode)\
	X(nudges)\
	X(controller_auto)\
	X(cheval_trigger)\
	X(tilt_learn_mode)\
	X(cheval_step)\
	X(joy_collector_pos)\
	X(set_button)

bool Teleop::operator<(Teleop const& a)const{
	#define X(NAME) if(NAME<a.NAME) return 1; if(a.NAME<NAME) return 0;
	TELEOP_ITEMS_NO_TYPE(X)
	#undef X
	return 0;
}

bool Teleop::operator==(Teleop const& a)const{
	#define X(NAME) if(NAME!=a.NAME) return 0;
	TELEOP_ITEMS_NO_TYPE(X)
	#undef X
	return 1;
}

void Teleop::display(ostream& o)const{
	o<<"Teleop( ";
	#define X(NAME) o<<""#NAME<<":"<<(NAME)<<" ";
	//TELEOP_ITEMS_NO_TYPE(X)
	#undef X
	o<<")";
}

#ifdef TELEOP_TEST
#include "test.h"

int main() {
	Teleop a;
	test_executive(a);
}
#endif
