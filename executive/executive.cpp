#include "executive.h"
#include<queue>
#include "teleop.h"

using namespace std;

//TODO: Move to util
vector<size_t> range(size_t lim){
	vector<size_t> r;
	for(size_t i=0;i<lim;i++) r|=i;
	return r;
}

template<typename T>
set<T> to_set(vector<T> v){
	set<T> r;
	for(auto a:v) r|=a;
	return r;
}

template<typename T>
vector<T> take(size_t lim,vector<T> v){
	vector<T> r;
	for(size_t i=0;i<lim && i<v.size();i++){
		r|=v[i];
	}
	return r;
}

void print_lines(auto a){
	for(auto elem:a){
		cout<<elem<<"\n";
	}
}

auto cross(auto a,auto b){
	using A=decltype(*begin(a));
	using B=decltype(*begin(b));
	vector<pair<A,B>> r;
	for(auto a1:a) for(auto b1:b){
		r|=make_pair(a1,b1);
	}
	return r;
}

//todo: Move to util
template<typename T>
set<T> operator-(set<T>,set<T>){
	nyi
}

//TODO: Move to util
template<typename Func,typename Collection>
Collection filter(Func f,Collection c){
	Collection r;
	for(auto a:c){
		if(f(a)) r|=a;
	}
	return r;
}

template<typename T>
vector<T> operator-(vector<T> a,set<T> b){
	return filter([b](auto elem){ return b.count(elem)==0; },a);
}

//todo: move to util
template<typename Func,typename Collection>
auto mapf(Func f,Collection c){
	using T=decltype(f(*begin(c)));
	vector<T> out;
	for(auto a:c) out|=f(a);
	return out;
}

IMPL_STRUCT(Next_mode_info::Next_mode_info,NEXT_MODE_INFO_ITEMS)
IMPL_STRUCT(Run_info::Run_info,RUN_INFO_ITEMS)

Executive& Executive::operator=(Executive const& a){
	assert(a.impl);
	impl=a.impl->clone();
	return *this;
}

Executive::Executive(Executive const& a){
	assert(a.impl);
	impl=a.impl->clone();
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
	return a.get()<b.get();
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

vector<Next_mode_info> examples(Next_mode_info *a){
	return mapf([&](int){ return example(a); },range(100));
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

vector<Run_info> examples(Run_info *a){
	return mapf([=](int){ return rand(a); },range(100));
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

bool operator<(unique_ptr<Executive> const& a,unique_ptr<Executive> const& b){
	if(a.get()){
		if(b.get()){
			return *a<*b;
		}
		return 0;
	}
	return a.get()<b.get();
}

template<typename T>
queue<T>& operator|=(queue<T> &q,T t){
	q.push(t);
	return q;
}

//This should probably go in its own file.
template<typename T>
class Unique_queue{
	set<T> s;
	queue<T> q;

	public:
	void insert(T t){
		auto p=s.insert(t);
		if(p.second){
			try{
				q|=t;
			}catch(...){
				//could decide to try to do something better w/ exception, like remove from the set then rethrow
				abort();
			}
		}
	}

	T pop(){
		try{
			auto r=q.front();
			q.pop();
			s.erase(r);
			return r;
		}catch(...){
			//no, throwing exceptions is not supported in this.
			abort();
		}
	}

	bool empty()const{
		return q.empty();
	}
};

template<typename T>
Unique_queue<T> &operator|=(Unique_queue<T> &a,T t){
	a.insert(t);
	return a;
}

template<typename T>
Unique_queue<T> &operator|=(Unique_queue<T> &a,vector<T> v){
	for(auto elem:v) a|=elem;
	return a;
}

//TODO: Move to util?
template<typename Node,typename F>
auto traverse(Node node,F next){
	Unique_queue<Node> to_visit;
	to_visit|=node;

	set<Node> visited;

	//Just chose some number that probably means that there's something depending on some floats.
	static const size_t MAX_STATES=100;
	while(!to_visit.empty() && visited.size()<MAX_STATES){
		auto at=to_visit.pop();
		visited|=at;

		//cout<<"Visiting:"<<at<<"\n";
		auto n=next(at);
		to_visit|=(n-visited);
	}

	//cout<<"Visited "<<visited.size()<<"\n";
	return visited;
}

void print_diff(Executive const& a,Executive const& b){
	if(a==b) return;
	a.get().diff(cout,b.get());
}

void show(string name,auto a){
	cout<<name<<":\n";
	for(auto elem:take(20,to_vec(to_set(a)))){
		cout<<"\t"<<elem<<"\n";
	}
}

void compare(set<Toplevel::Goal> s){
	#define X(A,B,C) show(""#A,mapf([](auto a){ return a.B; },s));
	TOPLEVEL_ITEMS
	#undef X
}

void test_mode(Executive mode){
	assert(mode==mode);
	assert(!(mode<mode));
	/*Things that could try doing:
	-see what the next modes show up as being if run a bunch of different stuff through
	-see if the internal state changes when different things happend to it & try to explore the state space
	-see what the Goals may be when a bunch of random stuff is run through.
	*/
	mode.next_mode(rand((Next_mode_info*)0));
	assert(mode==mode);
	auto a=mode;

	/*set<Executive> nexts;
	//should the next_mode function be const?
	for(auto _:range(100)){
		(void)_;
		auto n=mode.next_mode(rand((Next_mode_info*)0));
		nexts.insert(move(n));
	}
	PRINT(nexts.size());
	PRINT(nexts);*/
	auto nexts=traverse(
		mode,
		[&](Executive at){
			return mapf(
				[&](Next_mode_info next_mode_info){
					auto r=at.next_mode(next_mode_info);
					/*if(r!=at){
						print_diff(r,at);
					}*/
					return r;
				},
				examples((Next_mode_info*)0)
			);
		}
	);
	cout<<"Nexts:\n";
	print_lines(nexts);

	PRINT(mode);
	assert(
		mode.next_mode(Next_mode_info{false,false,rand((Toplevel::Status_detail*)0),0.0,rand((Panel*)0),rand((Robot_inputs*)0)}) == Executive{Teleop{}}
	);//test to make sure that each mode goes to Teleop if it's not in auto mode. Maybe expand this test later.
	
	set<Toplevel::Goal> outs;
	auto after=traverse(
		mode,
		[&](Executive at){
			return mapf(
				[&](Run_info run_info){
					auto b=at;
					outs|=b.run(run_info);
					return b;
				},
				examples((Run_info*)0)
			);
		}
	);

	//cross(nexts,examples((Run_info*)0));

	/*for(auto _:range(100)){
		(void)_;
		auto out=mode.run(rand((Run_info*)0));
		outs|=out;
	}*/
	//could do something more sophisticated to show what the changes are, per subsystem
	//PRINT(outs);
	compare(outs);

	set<Executive> after_run;
	mode.run(rand((Run_info*)0));
	if(a!=mode){
		//assert(0);//acutally should implement something
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
