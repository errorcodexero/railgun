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
	X(bool,toplready)\
	X(Robot_inputs,in)\
	X(SINGLE_ARG(std::pair<int,int>),initial_encoders)
DECLARE_STRUCT(Next_mode_info,NEXT_MODE_INFO_ITEMS)

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

struct Mode {
	virtual std::unique_ptr<Mode> next_mode(Next_mode_info)=0;
	virtual Toplevel::Goal run(Run_info)=0;

	virtual std::unique_ptr<Mode> clone()const=0;
	virtual bool operator<(Mode const&)const=0;
	virtual bool operator==(Mode const&)const=0;
	virtual void display(std::ostream&)const=0;
};

bool operator!=(Mode const&,Mode const&);
std::ostream& operator<<(std::ostream&,Mode const&);

void test_mode(Mode&);

template<typename T>
struct Mode_impl:Mode{
	T const& self()const{ return static_cast<T const&>(*this); }

	std::unique_ptr<Mode> clone()const{
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
		T const& b=dynamic_cast<T const&>(a);
		return this->operator<(b);
	}

	virtual bool operator<(T const& t)const{
		if(*this==t) return 0;
		assert(0);//FIXME
	}

	bool operator==(Mode const& a)const{
		const T *b=dynamic_cast<T const*>(&a);
		if(!b) return 0;
		return this->operator==(*b);
	}

	virtual bool operator==(T const&)const=0;

	void display(std::ostream& o)const{
		o<<type(self());
	}
};

#endif
