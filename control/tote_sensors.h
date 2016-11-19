#ifndef TOTE_SENSORS
#define TOTE_SENSORS

#include "nop.h"

struct Tote_sensors{
	struct Input{
		bool left,right,center;
	};

	struct Input_reader{
		Input operator()(Robot_inputs const&)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};
	Input_reader input_reader;

	using Output=Nop::Output;
	using Status_detail=Nop::Status_detail;
	using Status=Nop::Status;
	using Goal=Nop::Goal;
	using Estimator=Nop::Estimator;
	using Output_applicator=Nop::Output_applicator;

	Estimator estimator;
	Output_applicator output_applicator;
};

bool operator<(Tote_sensors::Input const&,Tote_sensors::Input const&);
bool operator==(Tote_sensors::Input const&,Tote_sensors::Input const&);
bool operator!=(Tote_sensors::Input const&,Tote_sensors::Input const&);
std::ostream& operator<<(std::ostream&,Tote_sensors::Input const&);

bool operator!=(Tote_sensors const&,Tote_sensors const&);
std::ostream& operator<<(std::ostream&,Tote_sensors const&);

#endif
