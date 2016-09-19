#include "step.h"
#include<queue>
#include "executive.h"
#include "../util/util.h"
#include "teleop.h"

using namespace std;

//this part should eventually go into util
template<typename T>
queue<T>& operator|=(queue<T> &q,T t){
	q.push(t);
	return q;
}

ostream& operator<<(ostream& o,Step const& a){
	a.display(o);
	return o;
}

struct Drive_straight:Step_impl_inner<Drive_straight>{
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	unique_ptr<Step_impl> clone()const;
	bool operator==(Drive_straight const&)const;
};

struct Turn:Step_impl{
	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	//Step clone()const;
};

struct Chain:Executive_impl<Chain>{
	Step step;
	Executive next;

	Chain(Step,Executive);

	Toplevel::Goal run(Run_info);
	Executive next_mode(Next_mode_info);
	bool operator==(Chain const&)const;
	unique_ptr<Executive_interface> clone()const;
};

//This part stays in the CPP file.

//Step::Step(Step_impl const& a):impl(a.clone().get()){}
Step::Step(Step_impl const& a){
	auto c=a.clone();
	if(!c)nyi
	impl=move(c);
}

bool Step::done(Next_mode_info a){
	return impl->done(a);
}

void Step::display(ostream& o)const{
	impl->display(o);
}

bool Step::operator==(Step const& a)const{
	assert(impl);
	return impl->operator==(a);
}

bool Step::operator<(Step const& a)const{
	assert(impl);
	return impl->operator<(a);
}

void Step_impl::display(ostream& o)const{
	o<<"Step_impl";
}

Step_impl::~Step_impl(){}

/*bool Step_impl::operator==(Step_impl const& a)const{
	T const& b=dynamic_cast<T const&>(a.get());
	return this->operator==(b);
}*/

bool Drive_straight::done(Next_mode_info){
	return 0;
}

Toplevel::Goal Drive_straight::run(Run_info){
	nyi
}

unique_ptr<Step_impl> Drive_straight::clone()const{
	return unique_ptr<Step_impl>(new Drive_straight(*this));
}

bool Drive_straight::operator==(Drive_straight const&)const{
	nyi
}

Chain::Chain(Step a,Executive next1):step(a),next(next1){}

/*Chain::Chain(Chain const& a):step(a.step){
	if(a.next)nyi
}*/

Toplevel::Goal Chain::run(Run_info){
	return Toplevel::Goal{};
}

Executive Chain::next_mode(Next_mode_info a){
	if(step.done(a)){
		/*if(next) return next->clone();
		return unique_ptr<Mode>();*/
		return next;
	}
	return Executive{*this};
}

bool Chain::operator==(Chain const&)const{
	return 1;
}

unique_ptr<Executive_interface> Chain::clone()const{
	return unique_ptr<Executive_interface>(new Chain(*this));
	//return make_unique<Chain>(step);
	//assert(0);
}

Step_impl const& Step::get()const{
	assert(impl);
	return *impl.get();
}

Step::Step(Step const& a):impl(a.get().clone()){}

/*
to put in its own file:

*/
class Do_list:public Step_impl_inner<Do_list>{
	vector<Step> steps;
	size_t index;

	public:
	explicit Do_list(vector<Step>);

	virtual Toplevel::Goal run(Run_info);
	virtual bool done(Next_mode_info);
	//virtual unique_ptr<Step_impl> clone()const;
	//virtual void display(ostream&)const;
	bool operator==(Do_list const&)const;
};

Do_list::Do_list(vector<Step> v):steps(v),index(0){}

Toplevel::Goal Do_list::run(Run_info){
	nyi
}

bool Do_list::operator==(Do_list const& a)const{
	return steps==a.steps && index==a.index;
}

bool Do_list::done(Next_mode_info){
	nyi
}

ostream& operator<<(ostream& o,Step_impl const& a){
	a.display(o);
	return o;
}

#ifdef STEP_TEST
void test_step(Step a){
	PRINT(a);
	auto b=a;
	assert(b==a);
	//see that can at least run "run", and "done"
	//a.run(example
	//todo: try to visit all the possible states that you can get to from the initial one, and see that things don't fail
	set<Step> visited;
	queue<Step> to_visit;
	to_visit|=a;

	//might want to put some limit on the amount of state space that's used.
	//todo: make a list of the inputs to try
	//first, run all the "run"s
	//second, run ready on all the different things and see if any of them change
	//abort if they do change

	while(to_visit.size()){
		auto f=to_visit.front();
		to_visit.pop();

		if(visited.find(f)!=visited.end()) continue;

		visited|=f;

		//TODO: Actually run some data on it.
	}

	//see that will at some point "done" can be set
}

template<typename T>
void test_step(T t){
	return test_step(Step{t});
}

int main(){
	Drive_straight a;
	Chain b{Step{a},Executive{Teleop{}}};
	Chain c{b};
	auto d=c.next_mode(example((Next_mode_info*)0));
	PRINT(c.step);
	PRINT(d);

	Do_list dl{{}};
	//PRINT(dl);
	test_step(dl);
}
#endif
