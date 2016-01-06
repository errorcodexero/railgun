#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/driver_station_interface.h"
#include "../util/util.h"

using namespace std;

struct Collector{
	enum class Goal{FORWARD,OFF,REVERSE}; 
	
	struct Status_detail{};

	typedef Status_detail Status;

	struct Input{};
	
	struct Input_reader{
		Collector::Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Collector::Input)const;
	};

	typedef double Output;
	
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Collector::Output)const;
		Collector::Output operator()(Robot_outputs)const;	
	};

	struct Estimator{
		Status_detail get()const;
		void update(Time,Input,Output){};	
	};

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator; 
};

ostream& operator<<(ostream&,Collector::Goal);
ostream& operator<<(ostream&,Collector);
ostream& operator<<(ostream&,Collector::Status_detail);
ostream& operator<<(ostream&,Collector::Input);

bool operator==(Collector::Input,Collector::Input);
bool operator!=(Collector::Input,Collector::Input);
bool operator<(Collector::Input, Collector::Input);
bool operator<(Collector::Status_detail, Collector::Status_detail);
bool operator==(Collector::Status_detail, Collector::Status_detail);
bool operator!=(Collector::Status_detail, Collector::Status_detail);
bool operator<(Collector::Input_reader, Collector::Input_reader);
bool operator==(Collector::Estimator, Collector::Estimator);
bool operator!=(Collector::Estimator, Collector::Estimator);
bool operator==(Collector::Input_reader,Collector::Input_reader);
bool operator==(Collector::Output_applicator,Collector::Output_applicator);
bool operator==(Collector,Collector);
bool operator!=(Collector,Collector);

set<Collector::Input> examples(Collector::Input*);
set<Collector::Goal> examples(Collector::Goal*);
set<Collector::Status_detail> examples(Collector::Status_detail*);
set<Collector::Output> examples(Collector::Output*);

Collector::Output control(Collector::Status_detail, Collector::Goal);
Collector::Status status(Collector::Status_detail);
bool ready(Collector::Status, Collector::Goal);

#endif
