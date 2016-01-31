#ifndef SHOOTER_H
#define SHOOTER_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/util.h"
#include "../util/driver_station_interface.h"

struct Shooter{
	enum class Goal{};
	
	struct Status_detail{};
	
	typedef Status_detail Status;

	struct Input{};
	
	struct Input_reader{
		Shooter::Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Shooter::Input)const;
	};

	typedef Goal Output;
	
	struct Output_applicator{
		Shooter::Output operator()(Robot_outputs)const;
		Robot_outputs operator()(Robot_outputs,Shooter::Output)const;
	};

	struct Estimator{
		Shooter::Status_detail get()const;
		void update(Time,Shooter::Input,Shooter::Output);
	};

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator;
};

std::ostream& operator<<(std::ostream&,Shooter::Goal);
std::ostream& operator<<(std::ostream&,Shooter::Input);
std::ostream& operator<<(std::ostream&,Shooter::Status_detail);
std::ostream& operator<<(std::ostream&,Shooter);

bool operator==(Shooter::Input,Shooter::Input);
bool operator!=(Shooter::Input,Shooter::Input);
bool operator<(Shooter::Input,Shooter::Input);

bool operator<(Shooter::Status_detail,Shooter::Status_detail);
bool operator==(Shooter::Status_detail,Shooter::Status_detail);
bool operator!=(Shooter::Status_detail,Shooter::Status_detail);

bool operator==(Shooter::Input_reader,Shooter::Input_reader);
bool operator<(Shooter::Input_reader,Shooter::Input_reader);

bool operator==(Shooter::Estimator,Shooter::Estimator);
bool operator!=(Shooter::Estimator,Shooter::Estimator);

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator);

bool operator==(Shooter,Shooter);
bool operator!=(Shooter,Shooter);

std::set<Shooter::Input> examples(Shooter::Input*);
std::set<Shooter::Goal> examples(Shooter::Goal*);
std::set<Shooter::Status_detail> examples(Shooter::Status_detail*);

Shooter::Output control(Shooter::Status_detail,Shooter::Goal);
Shooter::Status status(Shooter::Status_detail);
bool ready(Shooter::Status,Shooter::Goal);


#endif
