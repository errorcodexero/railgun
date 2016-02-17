#ifndef	WINCH_H
#define WINCH_H

#include <set>
#include "../util/interface.h"

struct Winch{
	enum class Goal{IN,OUT,STOP};
	using Output=Goal;
	struct Status{};
	using Status_detail=Status;
	using Input=Status;

	struct Input_reader{
		Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};

	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Output)const;
		Output operator()(Robot_outputs)const;
	};

	struct Estimator{
		void update(Time,Input,Output);
		Status_detail get()const;
	};

	Input_reader input_reader;
	Output_applicator output_applicator;
	Estimator estimator;
};

std::set<Winch::Status> examples(Winch::Status*);
Winch::Status status(Winch::Status);
bool ready(Winch::Status,Winch::Goal);

bool operator==(Winch::Status,Winch::Status);
std::ostream& operator<<(std::ostream&,Winch const&);
std::ostream& operator<<(std::ostream&,Winch::Goal);
std::ostream& operator<<(std::ostream&,Winch::Status);
bool operator<(Winch::Status,Winch::Status);
bool operator!=(Winch::Status,Winch::Status);
bool operator!=(Winch,Winch);
bool operator!=(Winch::Estimator,Winch::Estimator);

Winch::Output control(Winch::Status,Winch::Goal);

#endif
