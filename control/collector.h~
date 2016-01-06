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
	Goal goal; 
	
	struct Status_detail{};
	Status_detail status_detail;

	typedef Status_detail Status;
	Status status;

	struct Input{};
	Input input;
	
	struct Input_reader{
		Collector::Input operator()(Robot_inputs)const{ return Collector::Input{};}
		Robot_inputs operator()(Robot_inputs a,Collector::Input)const{ return a;}
	};
	Input_reader input_reader;

	typedef double Output;
	
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs r,Collector::Output out)const{ 
			r.pwm[3]=out;
			return r;
		}
		Collector::Output operator()(Robot_outputs r)const{ 
			return r.pwm[3]; 
		}	
	};
	Output_applicator output_applicator;

	struct Estimator{
		Status_detail get()const{ return Status_detail{};}
		void update(Time,Input,Output){};	
	};
	Estimator estimator;
	
	Collector():goal(Goal::OFF){}
};

ostream& operator<<(ostream&,Collector::Goal);
ostream& operator<<(ostream&,Collector);
ostream& operator<<(ostream&,Collector::Status_detail);
ostream& operator<<(ostream&,Collector::Input);

bool operator==(Collector::Input,Collector::Input);
bool operator<(Collector::Input, Collector::Input);
bool operator<(Collector::Status_detail, Collector::Status_detail);
bool operator==(Collector::Status_detail, Collector::Status_detail);
bool operator<(Collector::Input_reader, Collector::Input_reader);

set<Collector::Input> examples(Collector::Input*);
set<Collector::Goal> examples(Collector::Goal*);
set<Collector::Status_detail> examples(Collector::Status_detail*);
set<Collector::Output> examples(Collector::Output*);

Collector::Output control(Collector::Status_detail, Collector::Goal);
Collector::Status status(Collector::Status_detail);
bool ready(Collector::Status, Collector::Goal);

#endif
