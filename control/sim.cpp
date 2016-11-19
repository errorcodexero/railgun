#ifdef SIM_TEST
#include "toplevel.h"
#include "../util/type.h"

using namespace std;

//TODO: MOVE TO UTIL
template<typename T>
vector<pair<size_t,T>> enumerate(vector<T> a){
	vector<pair<size_t,T>> r;
	for(size_t i=0;i<a.size();i++){
		r|=make_pair(i,a[i]);
	}
	return r;
}

template<typename T,size_t LEN>
array<pair<size_t,T>,LEN> enumerate(array<T,LEN> a){
	array<pair<size_t,T>,LEN> r;
	for(auto i:range(LEN)){
		r[i]=make_pair(i,a[i]);
	}
	return r;
}

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

	//speed is in RPM
	#define SHOOTER_SIM_ITEMS(X)\
		X(double,speed,0)\
		X(bool,enabled,0)

	#define X(A,B,C) A B=C;
	SHOOTER_SIM_ITEMS(X)
	#undef X

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

template<typename T>
ostream& operator<<(ostream& o,Nop_sim<T> const& a){
	return o<<type(a);
}

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

template<typename SIMULATOR,typename DEVICE>
void simulate(SIMULATOR sim,DEVICE device){
	static const Time TIMESTEP=.1;//this will probably change to .01 or .02 later.
	auto in=sim.get();
	for(Time t=0;t<2;t+=TIMESTEP){
		//TODO: Make the simulator go 2014 style where you only get "significant" changes printed
		//device.estimator.update(
		nyi
	}
}

template<typename F>
void visit(F f,Toplevel_sim a){
	#define X(A,B,C) f(""#B,a.B);
	TOPLEVEL_ITEMS
	#undef X
}

template<typename F>
void visit(F f,Collector_sim a){
	#define X(A,B) f(""#B,a.B);
	COLLECTOR_ITEMS(X)
	#undef X
}

void visit(auto f,Shooter_sim a){
	#define X(A,B,C) f(""#B,a.B);
	SHOOTER_SIM_ITEMS(X)
	#undef X
}

template<typename F,typename T>
void visit(F f,T t){
	f.terminal(t);
}

template<typename F,typename T>
void visit(F,Nop_sim<T>){}

/*void visit(auto f,Encoder_info const& a){

}*/

template<typename T,size_t LEN>
void visit(auto f,array<T,LEN> const& a){
	//mapf([&](auto p){ f(as_string(p.first),p.second); },enumerate(a));
	for(auto p:enumerate(a)){
		f(as_string(p.first),p.second);
	}
}

template<typename A,typename B>
void visit(auto f,pair<A,B> p){
	f("first",p.first);
	f("second",p.second);
}

#define VISIT(A,B) f(""#B,a.B);

void visit(auto f,Drivebase::Input const& a){
	DRIVEBASE_INPUT(VISIT)
}

template<typename F>
void visit(F f,Toplevel::Input const& a){
	#define X(A,B,C) f(""#B,a.B);
	TOPLEVEL_ITEMS
	#undef X
}

void visit(auto f,Collector::Input const& a){
	COLLECTOR_ITEMS(VISIT)
}


void visit(auto f,Climb_release::Input const& a){
	#define X(A,B) f(""#B,a.B);
	X("",enabled)
	#undef X
}

void visit(auto f,Tilt::Status_detail const& a){
	VISIT(0,top)
	VISIT(0,angle)
}

void visit(auto f,Shooter::Input const& a){
	SHOOTER_INPUT_ITEMS(VISIT)
}

void visit(auto f,Winch::Input const& a){
	WINCH_INPUT_ITEMS(VISIT)
}

void visit(auto f,Front::Input a){
	FRONT_INPUT_ITEMS(VISIT)
}

void visit(auto f,Nop::Input){}

struct Display{
	int indent_level=0;

	string get_label(auto a)const{
		return type(a);
	}

	/*auto get_label(int a)const{ return to_string(a); }
	auto get_label(bool a)const{ return to_string(a); }
	auto get_label(double a)const{ return to_string(a); }*/
	
	void indent()const{
		for(auto _:range(indent_level)){
			(void)_;
			cout<<"\t";
		}
	}

	void operator()(string label,auto a)const{
		indent();
		cout<<label<<"\n";
		visit(Display{indent_level+1},a);
	}

	void terminal(auto a)const{
		indent();
		cout<<a<<"\n";	
	}
};

template<typename T>
void sim_display(T t){
	/*auto label=get_top(t);
	cout<<label<<"\n";*/
	Display{}("robot",t);
}

int main(){
	Toplevel_sim sim;
	PRINT(examples((Front::Goal*)0));
	//sim_display(sim);
	//sim_display(sim.get());
	//sim_display(example((Toplevel::Output*)0));

	/*static const Time TIMESTEP=.1;
	for(Time t=0;t<2;t+=TIMESTEP){
		cout<<t<<"\t"<<sim.get()<<"\n";
		auto out=example((Toplevel::Output*)0);
		sim.update(t,1,out);
	}
	return 0;*/
}

#endif
