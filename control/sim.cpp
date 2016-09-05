#ifdef SIM_TEST
#include "toplevel.h"

using namespace std;

#define nyi { cout<<"nyi "<<__LINE__<<"\n"; exit(44); }

/*Drivebase
Pump (full, estimator=pass-thru)
Collector
	front (input=ball preset?, status is a pass-through)
	sides (input=empty)
	tilt (done)
Climb_release - seems to have an input called "enabled", but that's it.  Status is IN,OUT,PROBABLY_OUT,UNKNOWN
Winch - has an input called deployed, no idea where this is coming from
Shooter (input(speed RPM,enabled)->speed in RPM)
*/

//This will probably go away later.
struct Shooter_sim{
	using Input=Shooter::Input;
	using Output=Shooter::Output;

	double speed=0;//rpm
	bool enabled=0;

	//enabled can't be totally abstracted away since it get directly passed into some of the estimators.
	//at some point, should just make it so that the estimators don't get it an instead they just get what the effective output is because of the robot being disabled.
	//Also, at some point when we're going between the modes we'll also need to know when we're in autonomous vs. in teleop, etc.
	void update(Time /*timestep*/,bool enabled,Output out){
		this->enabled=enabled;
		if(!enabled){
			out=Shooter::Output();
			nyi//out.mode=Talon_srx_output::Mode 
		}
		//nyi
	}

	Input get()const{
		return Input{int(speed),enabled};
	}
};

template<typename Input>
struct Nop_sim{
	template<typename Output>
	void update(Time,bool,Output){}

	Input get()const{
		auto e=examples((Input*)0);
		assert(e.size());
		return *begin(e);
	}
};

using Sides_sim=Nop_sim<Sides::Input>;
using Front_sim=Nop_sim<Front::Input>;
using Tilt_sim=Nop_sim<Tilt::Input>;

struct Collector_sim{
	using Input=Collector::Input;
	using Output=Collector::Output;

	#define X(A,B) A##_sim B;
	COLLECTOR_ITEMS(X)
	#undef X

	void update(Time t,bool enable,Output out){
		#define X(A,B) B.update(t,enable,out.B);
		COLLECTOR_ITEMS(X)
		#undef X
	}

	Input get()const{
		return Input{
			#define X(A,B) B.get(),
			COLLECTOR_ITEMS(X)
			#undef X
		};
	}
};

using Drivebase_sim=Nop_sim<Drivebase::Input>;
using Pump_sim=Nop_sim<Pump::Input>;
using Climb_release_sim=Nop_sim<Climb_release::Input>;
using Winch_sim=Nop_sim<Winch::Input>;

struct Toplevel_sim{
	using Input=Toplevel::Input;
	using Output=Toplevel::Output;

	#define X(A,B,C) A##_sim B;
	TOPLEVEL_ITEMS
	#undef X

	void update(Time t,bool enable,Output out){
		#define X(A,B,C) B.update(t,enable,out.B);
		TOPLEVEL_ITEMS
		#undef X
	}

	Input get()const{
		return {
			#define X(A,B,C) B.get(),
			TOPLEVEL_ITEMS
			#undef X
		};
	}
};

int main(){
	Toplevel_sim sim;
	static const Time TIMESTEP=.1;
	for(Time t=0;t<2;t+=TIMESTEP){
		cout<<t<<"\t"<<sim.get()<<"\n";
		auto out=example((Toplevel::Output*)0);
		sim.update(t,1,out);
	}
	return 0;
}

#endif
