#ifndef COLLECTOR_H
#define COLLECTOR_H

#include "front.h"
#include "sides.h"
#include "tilt.h"

struct Collector{
	#define COLLECTOR_ITEMS(X) X(Front,front) X(Sides,sides) X(Tilt,tilt)

	struct Input{
		#define X(A,B) A::Input B;
		COLLECTOR_ITEMS(X)
		#undef X
	};

	struct Output{
		#define X(A,B) A::Output B;
		COLLECTOR_ITEMS(X)
		#undef X
	};

	struct Goal{
		#define X(A,B) A::Goal B;
		COLLECTOR_ITEMS(X)
		#undef X
	};

	struct Input_reader{
		#define X(A,B) A::Input_reader B;
		COLLECTOR_ITEMS(X)
		#undef X

		Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};

	struct Output_applicator{
		#define X(A,B) A::Output_applicator B;
		COLLECTOR_ITEMS(X)
		#undef X

		Output operator()(Robot_outputs const&)const;
		Robot_outputs operator()(Robot_outputs,Output)const;
	};

	struct Status{
		#define X(A,B) A::Status B;
		COLLECTOR_ITEMS(X)
		#undef X
	};

	struct Status_detail{
		#define X(A,B) A::Status_detail B;
		COLLECTOR_ITEMS(X)
		#undef X
	};

	struct Estimator{
		#define X(A,B) A::Estimator B;
		COLLECTOR_ITEMS(X)
		#undef X

		void update(Time,Input,Output);
		Status_detail get()const;
	};

	Estimator estimator;
	Input_reader input_reader;
	Output_applicator output_applicator;
};

bool operator==(Collector::Input,Collector::Input);
bool operator<(Collector::Output,Collector::Output);
bool operator==(Collector::Output,Collector::Output);
bool operator!=(Collector::Output,Collector::Output);
bool operator<(Collector::Status,Collector::Status);
bool operator==(Collector::Status,Collector::Status);
bool operator<(Collector::Status_detail,Collector::Status_detail);

std::ostream& operator<<(std::ostream&,Collector::Input const&);
std::ostream& operator<<(std::ostream&,Collector::Output const&);
std::ostream& operator<<(std::ostream&,Collector::Status const&);
std::ostream& operator<<(std::ostream&,Collector::Status_detail const&);
std::ostream& operator<<(std::ostream&,Collector::Goal const&);
std::ostream& operator<<(std::ostream&,Collector const&);

Collector::Status status(Collector::Status_detail const&);
Collector::Output control(Collector::Status_detail const&,Collector::Goal const&);

#endif
