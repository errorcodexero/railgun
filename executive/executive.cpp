#include "executive.h"
#include "teleop.h"

using namespace std;

IMPL_STRUCT(Next_mode_info::Next_mode_info,NEXT_MODE_INFO_ITEMS)
IMPL_STRUCT(Run_info::Run_info,RUN_INFO_ITEMS)

Executive& Executive::operator=(Executive const& a){//Hey Eric, please make this right if it's wrong
	if(a.impl){
		impl=a.impl->clone();
	}
	return *this;
}

Executive::Executive(Executive const& a){
	if(a.impl){
		impl=a.impl->clone();
	}
}

Executive::Executive(Executive_interface const& a):impl(a.clone()){
	assert(impl);
}

Executive_interface const& Executive::get()const{
	auto g=impl.get();
	assert(g);
	return *g;
}

Executive Executive::next_mode(Next_mode_info a){
	assert(impl);
	return impl->next_mode(a);
}

Toplevel::Goal Executive::run(Run_info a){
	assert(impl);
	return impl->run(a);
}

bool operator<(Executive const& a,Executive const& b){
	return a.get()<(b);
}

bool operator==(Executive const& a,Executive const& b){
	assert(a.impl);
	return a.impl->operator==(b);
}

ostream& operator<<(ostream& o,Executive_interface const& a){
	a.display(o);
	return o;
}

ostream& operator<<(ostream& o,Executive const& a){
	return o<<*a.impl;
}

bool rand(bool*){
	return rand()%2;
}

Toplevel::Status_detail rand(Toplevel::Status_detail *a){
	auto e=examples(a);
	assert(e.size());
	return choose_random(e);
}

Time rand(Time*){
	//chosen arbitrarily.
	return Time(rand()%100);
}

Panel rand(Panel*){
	return interpret(driver_station_input_rand());
}

Robot_inputs rand(Robot_inputs*){
	return random_inputs();
}

pair<int,int> rand(pair<int,int>*){
	return make_pair(rand(),rand());
}

Next_mode_info rand(Next_mode_info*){
	return Next_mode_info{
		#define X(A,B) rand((A*)0),
		NEXT_MODE_INFO_ITEMS(X)
		#undef X
	};
}

Next_mode_info example(Next_mode_info *a){
	return rand(a);
}

Joystick_data rand(Joystick_data*){
	return driver_station_input_rand();
}

Tilt::Goal rand(Tilt::Goal *a){
	return choose_random(examples(a));
}

Run_info rand(Run_info*){
	return Run_info{
		#define X(A,B) rand((A*)0),
		RUN_INFO_ITEMS(X)
		#undef X
	};
}

/*bool operator==(Next_mode_info const& a,Next_mode_info const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	NEXT_MODE_INFO_ITEMS(X)
	#undef X
	return 1;
}

bool operator==(Run_info const& a,Run_info const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	RUN_INFO_ITEMS(X)
	#undef X
	return 1;
}*/

/*bool operator==(Executive const& a,Executive const& b){
	return a.equals(b);
}*/

bool operator!=(Executive const& a,Executive const& b){
	return !(a==b);
}

//TODO: Move to util
vector<size_t> range(size_t lim){
	vector<size_t> r;
	for(size_t i=0;i<lim;i++) r|=i;
	return r;
}

bool operator<(unique_ptr<Executive> const& a,unique_ptr<Executive> const& b){
	if(a.get()){
		if(b.get()){
			return *a<*b;
		}
		return 0;
	}
	return a.get()<b.get();
}

void test_mode(Executive mode){
	/*Things that could try doing:
	-see what the next modes show up as being if run a bunch of different stuff through
	-see if the internal state changes when different things happend to it & try to explore the state space
	-see what the Goals may be when a bunch of random stuff is run through.
	*/
	mode.next_mode(rand((Next_mode_info*)0));
	assert(mode==mode);
	auto a=mode;//.clone();
	//assert(a);

	set<Executive> nexts;
	//should the next_mode function be const?
	for(auto _:range(100)){
		(void)_;
		auto n=mode.next_mode(rand((Next_mode_info*)0));
		nexts.insert(move(n));
	}
	PRINT(nexts.size());
	PRINT(nexts);
	
	
	PRINT(mode);
	assert(
		mode.next_mode(Next_mode_info{false,false,rand((Toplevel::Status_detail*)0),0.0,rand((Panel*)0),rand((Robot_inputs*)0)}) == Executive{Teleop{}}
	);//test to make sure that each mode goes to Teleop if it's not in auto mode. Maybe expand this test later.
	
	set<Toplevel::Goal> outs;
	for(auto _:range(100)){
		(void)_;
		auto out=mode.run(rand((Run_info*)0));
		outs|=out;
	}
	//could do something more sophisticated to show what the changes are, per subsystem
	PRINT(outs);

	mode.run(rand((Run_info*)0));
	if(a!=mode){
		assert(0);//acutally should implement something
	}
}

#ifdef EXECUTIVE_TEST
int main(){
	Next_mode_info *a(0);
	Run_info *b=0;

	rand(a);
	rand(b);
}
#endif
