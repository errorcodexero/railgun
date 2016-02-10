#ifndef INOUT_H
#define INOUT_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/util.h"
#include "../util/driver_station_interface.h"

struct Inout{
	enum class Goal{};
	
	struct Status_detail{};
	
	typedef Status_detail Status;

	struct Input{};
	
	struct Input_reader{
		Inout::Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Inout::Input)const;
	};

	typedef Goal Output;
	
	struct Output_applicator{
		Inout::Output operator()(Robot_outputs)const;
		Robot_outputs operator()(Robot_outputs,Inout::Output)const;
	};

	struct Estimator{
		Inout::Status_detail get()const;
		void update(Time,Inout::Input,Inout::Output);
	};

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator;
};

std::ostream& operator<<(std::ostream&,Inout::Goal);
std::ostream& operator<<(std::ostream&,Inout::Input);
std::ostream& operator<<(std::ostream&,Inout::Status_detail);
std::ostream& operator<<(std::ostream&,Inout);

bool operator==(Inout::Input,Inout::Input);
bool operator!=(Inout::Input,Inout::Input);
bool operator<(Inout::Input,Inout::Input);

bool operator<(Inout::Status_detail,Inout::Status_detail);
bool operator==(Inout::Status_detail,Inout::Status_detail);
bool operator!=(Inout::Status_detail,Inout::Status_detail);

bool operator==(Inout::Input_reader,Inout::Input_reader);
bool operator<(Inout::Input_reader,Inout::Input_reader);

bool operator==(Inout::Estimator,Inout::Estimator);
bool operator!=(Inout::Estimator,Inout::Estimator);

bool operator==(Inout::Output_applicator,Inout::Output_applicator);

bool operator==(Inout,Inout);
bool operator!=(Inout,Inout);

std::set<Inout::Input> examples(Inout::Input*);
std::set<Inout::Goal> examples(Inout::Goal*);
std::set<Inout::Status_detail> examples(Inout::Status_detail*);

Inout::Output control(Inout::Status_detail,Inout::Goal);
Inout::Status status(Inout::Status_detail);
bool ready(Inout::Status,Inout::Goal);


#endif
