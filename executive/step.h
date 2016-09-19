#ifndef STEP_H
#define STEP_H

#include "executive.h"

struct Step_impl;

class Step{
	std::unique_ptr<Step_impl> impl;

	public:
	explicit Step(Step_impl const&);
	Step(Step const&);

	Toplevel::Goal run(Run_info);
	bool done(Next_mode_info);
	Step_impl const& get()const;
	void display(std::ostream&)const;
	bool operator==(Step const&)const;
	bool operator<(Step const&)const;
};

struct Step_impl{
	virtual ~Step_impl();

	virtual Toplevel::Goal run(Run_info)=0;
	virtual bool done(Next_mode_info)=0;//could try to make this const.
	virtual std::unique_ptr<Step_impl> clone()const=0;
	virtual void display(std::ostream&)const;
	virtual bool operator<(Step const&)const=0;
	virtual bool operator==(Step const&)const=0;
};

template<typename T>
struct Step_impl_inner:Step_impl{
	std::unique_ptr<Step_impl> clone()const{
		T const& t=dynamic_cast<T const&>(*this);
		return std::unique_ptr<Step_impl>(new T(t));
	}
	
	void display(std::ostream& o)const{
		o<<type(*(T*)this);
	}

	bool operator==(Step const& a)const{
		//Step_impl const& b=a.get();
		//if(type_index(typeid(*this))!=type_index
		T const& t=dynamic_cast<T const&>(a.get());
		return operator==(t);
	}

	virtual bool operator==(T const&)const=0;

	bool operator<(Step const&)const{
		nyi
	}
};

#endif
