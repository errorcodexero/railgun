#ifndef MODE_H
#define MODE_H

#include <memory>
#include <typeindex>
#include "../control/toplevel.h"
#include "../input/panel.h"
#include "../util/interface.h"
#include "../util/type.h"

#define NEXT_MODE_INFO_ITEMS(X)\
	X(bool,autonomous)\
	X(bool,autonomous_start)\
	X(Toplevel::Status_detail,status)\
	X(Time,since_switch)\
	X(Panel,panel)\
	X(Robot_inputs,in)
DECLARE_STRUCT(Next_mode_info,NEXT_MODE_INFO_ITEMS)
Next_mode_info example(Next_mode_info*);
std::ostream& operator<<(std::ostream&,Next_mode_info const&);

#define RUN_INFO_ITEMS(X)\
	X(Robot_inputs,in)\
	X(Joystick_data,main_joystick)\
	X(Joystick_data,gunner_joystick)\
	X(Panel,panel)\
	X(Toplevel::Status_detail,toplevel_status)\
	X(Tilt::Goal,level)\
	X(Tilt::Goal,low)\
	X(Tilt::Goal,top)\
	X(Tilt::Goal,cheval)\
	X(Tilt::Goal,drawbridge)
DECLARE_STRUCT(Run_info,RUN_INFO_ITEMS)
Run_info example(Run_info*);
std::ostream& operator<<(std::ostream&,Run_info const&);

class Mode_interface;

class Mode{
	std::unique_ptr<Mode_interface> impl;

	public:
	Mode(Mode const&);
	explicit Mode(Mode_interface const&);

	Mode next_mode(Next_mode_info);
	Toplevel::Goal run(Run_info);
	Mode_interface const& get()const;

	//bool operator<(Mode const&);
	Mode& operator=(Mode const&);
	friend bool operator==(Mode const&,Mode const&);
	friend std::ostream& operator<<(std::ostream&,Mode const&);
};
bool operator<(Mode const&,Mode const&);
bool operator==(Mode const&,Mode const&);

struct Mode_interface {
	virtual Mode next_mode(Next_mode_info)=0;
	virtual Toplevel::Goal run(Run_info)=0;

	virtual std::unique_ptr<Mode_interface> clone()const=0;
	virtual bool operator<(Mode const&)const=0;
	virtual bool operator==(Mode const&)const=0;
	virtual void display(std::ostream&)const=0;
};

bool operator!=(Mode const&,Mode const&);
std::ostream& operator<<(std::ostream&,Mode const&);

void test_mode(Mode);

template<typename T>
void test_mode(T a){
	return test_mode(Mode{a});
}

template<typename T>
struct Mode_impl:Mode_interface{
	T const& self()const{ return static_cast<T const&>(*this); }

	std::unique_ptr<Mode_interface> clone()const{
		return std::make_unique<T>(self());
	}

	bool operator<(Mode const& a)const{
		auto t1=std::type_index(typeid(*this));
		auto t2=std::type_index(typeid(a));
		if(t1<t2){
			return 1;
		}
		if(t1>t2){
			return 0;
		}
		nyi/*T const& b=dynamic_cast<T const&>(a);
		return this->operator<(b);*/
	}

	virtual bool operator<(T const& t)const{
		if(*this==t) return 0;
		assert(0);//FIXME
	}

	bool operator==(Mode const& a)const{
		T const& b=dynamic_cast<T const&>(a.get());
		return this->operator==(b);
	}

	virtual bool operator==(T const&)const=0;

	void display(std::ostream& o)const{
		o<<type(self());
	}
};

#endif
