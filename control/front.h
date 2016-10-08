#ifndef FRONT_H
#define FRONT_H

#include <iostream>
#include <set>
#include "../util/interface.h"

struct Front{
	#define FRONT_GOAL(X) X(IN) X(OFF) X(OUT) X(CLEAR_BALL)
	enum class Goal{
		#define X(A) A,
		FRONT_GOAL(X)
		#undef X
	};

	#define FRONT_INPUT_ITEMS(X) X(bool,ball)
	struct Input{
		bool ball;

		Input();
		explicit Input(bool);
	};
	
	typedef Input Status_detail;
	typedef Status_detail Status;

	struct Input_reader{
		Front::Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Front::Input)const;
	};

	struct Output{
		Goal motor;
		bool ball_light;
	};
	
	struct Output_applicator{
		Robot_outputs operator()(Robot_outputs,Front::Output)const;
		Front::Output operator()(Robot_outputs)const;	
	};

	struct Estimator{
		Input input;

		Estimator();
		Status_detail get()const;
		void update(Time,Input,Output);
	};

	Input_reader input_reader;
	Estimator estimator;
	Output_applicator output_applicator; 
};

std::ostream& operator<<(std::ostream&,Front::Goal);
std::ostream& operator<<(std::ostream&,Front);
std::ostream& operator<<(std::ostream&,Front::Input);
std::ostream& operator<<(std::ostream&,Front::Output);

bool operator==(Front::Input,Front::Input);
bool operator!=(Front::Input,Front::Input);
bool operator<(Front::Input, Front::Input);

bool operator==(Front::Output,Front::Output);
bool operator!=(Front::Output,Front::Output);
bool operator<(Front::Output, Front::Output);

bool operator==(Front::Input_reader,Front::Input_reader);
bool operator<(Front::Input_reader, Front::Input_reader);

bool operator==(Front::Estimator, Front::Estimator);
bool operator!=(Front::Estimator, Front::Estimator);

bool operator==(Front::Output_applicator,Front::Output_applicator);

bool operator==(Front,Front);
bool operator!=(Front,Front);

std::set<Front::Input> examples(Front::Input*);
std::set<Front::Output> examples(Front::Output*);
std::set<Front::Goal> examples(Front::Goal*);

Front::Output control(Front::Status_detail, Front::Goal);
Front::Status status(Front::Status_detail);
bool ready(Front::Status, Front::Goal);

#endif
