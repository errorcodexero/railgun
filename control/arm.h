#ifndef ARM_H
#define ARM_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/driver_station_interface.h"
#include "../util/util.h"

using namespace std;

struct Arm{
	enum class Goal{UP_AUTO,DOWN_AUTO,UP_MANUAL,DOWN_MANUAL,STOP};
	enum class Status{UP,MID,DOWN};

	typedef Status Status_detail;
	
	struct Input{
		bool topLimit, bottomLimit;
	};

	struct Input_reader{
		Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};

	enum class Output{UP,OFF,DOWN};

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs, Output)const;
		Output operator()(Robot_outputs)const;
	};

	struct Estimator{
		Status_detail last;

		Estimator():last(Status_detail::MID){}

		void update(Time t, Input in, Output out);
		Status_detail get()const;
	};
	Estimator estimator;
	Input_reader input_reader;
	//Goal goal;
	Output_applicator output_applicator;
};

ostream& operator<<(ostream&,Arm::Input);
ostream& operator<<(ostream&,Arm::Output_applicator);
ostream& operator<<(ostream&,Arm::Output);
ostream& operator<<(ostream&,Arm::Status);
ostream& operator<<(ostream&,Arm::Goal);
ostream& operator<<(ostream&,Arm::Estimator);
ostream& operator<<(ostream&,Arm);

bool operator==(Arm::Input const&,Arm::Input const&);
bool operator!=(Arm::Input const&,Arm::Input const&);
bool operator==(Arm::Estimator const&, Arm::Estimator const&);
bool operator!=(Arm::Estimator const&, Arm::Estimator const&);
bool operator==(Arm::Output_applicator const&,Arm::Output_applicator const&);
bool operator==(Arm const&,Arm const&);
bool operator!=(Arm const&,Arm const&);

bool operator<(Arm::Input,Arm::Input);

set<Arm::Input> examples(Arm::Input*);
set<Arm::Output> examples(Arm::Output*);
set<Arm::Goal> examples(Arm::Goal*);
set<Arm::Status> examples(Arm::Status*);

Arm::Output control(Arm::Status_detail,Arm::Goal goal);
Arm::Status status(Arm::Status_detail);
bool ready(Arm::Status,Arm::Goal);

#endif
