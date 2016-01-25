#ifndef CLIMB_H
#define CLIMB_H

#include <iostream>
#include <set>
#include "../util/util.h"
#include "../util/interface.h"
#include "../util/driver_station_interface.h"

struct Climb{
	enum class Goal{UP,STOP,DOWN};
	
	struct Status_detail{};

	typedef Status_detail Status;
	
	struct Input{};

	struct Input_reader{
		Climb::Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Climb::Input)const;
	};

	typedef Goal Output;

	struct Output_applicator{
		Climb::Output operator()(Robot_outputs)const;
		Robot_outputs operator()(Robot_outputs,Climb::Output)const;
	};
	
	struct Estimator{
		Climb::Status_detail get()const;
		void update(Time,Input,Output);
	};
	
	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator;
};

set<Climb::Input> examples(Climb::Input*);
set<Climb::Goal> examples(Climb::Goal*);
set<Climb::Status_detail> examples(Climb::Status_detail*);

#endif
