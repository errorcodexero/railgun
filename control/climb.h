#ifndef CLIMB_H
#define CLIMB_H

#include <iostream>
#include <set>
#include "../util/util.h"
#include "../util/interface.h"
#include "../util/driver_station_interface.h"

struct Climb{
	#define CLIMB_GOALS \
		X(EXTEND) \
		X(STOP) \
		X(RETRACT)
	#define X(name) name,
	enum class Goal{CLIMB_GOALS};
	#undef X
	
	class Status_detail{
		public:
		enum class Type{TOP,BOTTOM,MID,ERRORS};

		private:
		Status_detail();

		Type type_;

		public:
		Type type()const;

		static Status_detail top();
		static Status_detail mid();
		static Status_detail bottom();
		static Status_detail error();	
	};

	typedef Status_detail Status;
	
	struct Input{
		bool top,bottom;
	};

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
		Climb::Status_detail last;
		Estimator();
		Climb::Status_detail get()const;
		void update(Time,Input,Output);
	};
	
	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator;
};

std::ostream& operator<<(std::ostream&,Climb);
std::ostream& operator<<(std::ostream&,Climb::Goal);
std::ostream& operator<<(std::ostream&,Climb::Input);
std::ostream& operator<<(std::ostream&,Climb::Status_detail);

bool operator==(Climb::Input,Climb::Input);
bool operator!=(Climb::Input,Climb::Input);
bool operator<(Climb::Input,Climb::Input);

bool operator<(Climb::Goal,Climb::Goal);

bool operator==(Climb::Status_detail,Climb::Status_detail);
bool operator!=(Climb::Status_detail,Climb::Status_detail);
bool operator<(Climb::Status_detail,Climb::Status_detail);

bool operator==(Climb::Input_reader,Climb::Input_reader);
bool operator==(Climb::Output_applicator,Climb::Output_applicator);

bool operator==(Climb::Estimator,Climb::Estimator);
bool operator!=(Climb::Estimator,Climb::Estimator);

bool operator==(Climb,Climb);
bool operator!=(Climb,Climb);

std::set<Climb::Input> examples(Climb::Input*);
std::set<Climb::Goal> examples(Climb::Goal*);
std::set<Climb::Status_detail> examples(Climb::Status_detail*);

Climb::Output control(Climb::Status_detail,Climb::Goal);
Climb::Status status(Climb::Status_detail);
bool ready(Climb::Status,Climb::Goal);

#endif
