#ifndef SIDES_H
#define SIDES_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "nop.h"

struct Sides{
	enum class Goal{IN,OFF,OUT};
	
	using Input=Nop::Input;
	
	typedef Input Status_detail;

	typedef Status_detail Status;

	using Input_reader=Nop::Input_reader;

	typedef Goal Output;
	
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Sides::Output)const;
		Sides::Output operator()(Robot_outputs)const;	
	};

	using Estimator=Nop::Estimator;

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator; 
};

std::ostream& operator<<(std::ostream&,Sides::Goal);
std::ostream& operator<<(std::ostream&,Sides);

bool operator<(Sides::Status_detail,Sides::Status_detail);
bool operator==(Sides::Status_detail,Sides::Status_detail);
bool operator!=(Sides::Status_detail,Sides::Status_detail);

bool operator==(Sides::Output_applicator,Sides::Output_applicator);

bool operator==(Sides,Sides);
bool operator!=(Sides,Sides);

std::set<Sides::Goal> examples(Sides::Goal*);
std::set<Sides::Status_detail> examples(Sides::Status_detail*);

Sides::Output control(Sides::Status_detail, Sides::Goal);
Sides::Status status(Sides::Status_detail);
bool ready(Sides::Status, Sides::Goal);

#endif
