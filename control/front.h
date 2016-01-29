#ifndef FRONT_H
#define FRONT_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/driver_station_interface.h"
#include "../util/util.h"

using namespace std;

struct Front{
	enum class Goal{IN,OFF,OUT};
	
	struct Status_detail{
		bool has_ball;
		Status_detail();
	};

	typedef Status_detail Status;

	struct Input{
		bool has_ball;
	};
	
	struct Input_reader{
		Front::Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Front::Input)const;
	};

	typedef Goal Output;
	
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Front::Output)const;
		Front::Output operator()(Robot_outputs)const;	
	};

	struct Estimator{
		Status_detail get()const;
		void update(Time,Input,Output){};	
	};

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator; 
};

ostream& operator<<(ostream&,Front::Goal);
ostream& operator<<(ostream&,Front);
ostream& operator<<(ostream&,Front::Status_detail);
ostream& operator<<(ostream&,Front::Input);

bool operator==(Front::Input,Front::Input);
bool operator!=(Front::Input,Front::Input);
bool operator<(Front::Input, Front::Input);

bool operator<(Front::Status_detail, Front::Status_detail);
bool operator==(Front::Status_detail, Front::Status_detail);
bool operator!=(Front::Status_detail, Front::Status_detail);

bool operator==(Front::Input_reader,Front::Input_reader);
bool operator<(Front::Input_reader, Front::Input_reader);

bool operator==(Front::Estimator, Front::Estimator);
bool operator!=(Front::Estimator, Front::Estimator);

bool operator==(Front::Output_applicator,Front::Output_applicator);

bool operator==(Front,Front);
bool operator!=(Front,Front);

set<Front::Input> examples(Front::Input*);
set<Front::Goal> examples(Front::Goal*);
set<Front::Status_detail> examples(Front::Status_detail*);

Front::Output control(Front::Status_detail, Front::Goal);
Front::Status status(Front::Status_detail);
bool ready(Front::Status, Front::Goal);

#endif
