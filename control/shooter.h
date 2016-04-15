#ifndef SHOOTER_H
#define SHOOTER_H

#include <iostream>
#include <set>
#include "../util/interface.h"
#include "../util/util.h"
#include "../util/driver_station_interface.h"
#include "../util/countdown_timer.h"

struct Shooter{
	#define SHOOTER_GOALS X(STOP) X(GROUND_SHOT) X(CLIMB_SHOT) X(X)
	struct Goal{
		enum class Type{
			#define X(name) name,
			SHOOTER_GOALS
			#undef X
		};
		Talon_srx_output::Mode mode;
		Type type;
		float percentage;//varries goal by a percentage
		Goal();
		Goal(Shooter::Goal::Type);
		Goal(Talon_srx_output::Mode,Shooter::Goal::Type);
		void operator()(Talon_srx_output::Mode const&);
	};
	
	struct Status_detail{
		double speed;//rpm
		bool beam;//beam sensor	

		Status_detail();
		Status_detail(double,bool);
	};
	
	typedef Status_detail Status;

	struct Input{
		int speed;//rpm
		bool beam;//beam sensor
		bool enabled;
	};
	
	struct Input_reader{
		Shooter::Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Shooter::Input)const;
	};

	struct Output{//maybe replace with a full Talon_srx_output or have one within
		double speed;//rpm
		Talon_srx_output::Mode talon_mode;
		Output();
		Output(double);
		Output(double,Talon_srx_output::Mode);
	};
	
	struct Output_applicator{
		Shooter::Output operator()(Robot_outputs)const;
		Robot_outputs operator()(Robot_outputs,Shooter::Output)const;
	};

	struct Estimator{
		Shooter::Status_detail last;
		Countdown_timer speed_up_timer;
		Shooter::Output last_output;
	
		Shooter::Status_detail get()const;
		void update(Time,Shooter::Input,Shooter::Output);
		Estimator();
	};

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator;
};

std::ostream& operator<<(std::ostream&,Shooter::Goal);
std::ostream& operator<<(std::ostream&,Shooter::Input);
std::ostream& operator<<(std::ostream&,Shooter::Status_detail);
std::ostream& operator<<(std::ostream&,Shooter);
std::ostream& operator<<(std::ostream&,Shooter::Output);

bool operator==(Shooter::Input,Shooter::Input);
bool operator!=(Shooter::Input,Shooter::Input);
bool operator<(Shooter::Input,Shooter::Input);

bool operator<(Shooter::Status_detail,Shooter::Status_detail);
bool operator==(Shooter::Status_detail,Shooter::Status_detail);
bool operator!=(Shooter::Status_detail,Shooter::Status_detail);

bool operator==(Shooter::Goal,Shooter::Goal);
bool operator!=(Shooter::Goal,Shooter::Goal);
bool operator<(Shooter::Goal,Shooter::Goal);

bool operator==(Shooter::Output,Shooter::Output);
bool operator!=(Shooter::Output,Shooter::Output);
bool operator<(Shooter::Output,Shooter::Output);

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
std::set<Shooter::Status> examples(Shooter::Status*);

Shooter::Output control(Shooter::Status_detail,Shooter::Goal);
Shooter::Status status(Shooter::Status_detail);
bool ready(Shooter::Status,Shooter::Goal);


#endif
