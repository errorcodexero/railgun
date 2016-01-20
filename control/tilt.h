#ifndef TILT_H
#define TILT_H

#include <iostream>
#include "../util/util.h"
#include "../util/interface.h"
#include "../util/driver_station_interface.h"
#include "../util/countdown_timer.h"
#include <set>

struct Tilt{
	struct Goal{
		public: 
		enum class Mode{GO_TO_ANGLE,UP,DOWN,STOP};
		
		private:
		Goal();

		Mode mode_;
		double angle_min,angle_target,angle_max;	
	
		public:
		Mode mode()const;
		std::array<double,3> angle()const;
		
		static Goal up();
		static Goal down();
		static Goal go_to_angle(std::array<double,3>);
		static Goal stop();
	};
	
	typedef double Output;	
	
	struct Status_detail{
		public:
		enum class Type{TOP,BOTTOM,MID,ERRORS};
		std::pair<bool,bool> stalled;
		
		private:
		Status_detail();

		Type type_;
		double angle;
		
		public:
		Type type()const;
		double get_angle()const;
		
		static Status_detail top();
		static Status_detail mid(double);
		static Status_detail bottom();	
		static Status_detail error();	
	
	};

	typedef Status_detail Status;

	struct Input{
		bool top,bottom;
		double current;
		int ticks;
	};
	
	struct Estimator{
		Maybe_inline<int> top,bottom;
		Maybe_inline<double> range()const;

		public: 
		Estimator();
		
		Countdown_timer stall_timer;
		double timer_start_angle;
		
		void update(Time,Input,Output);
		Status_detail get()const;
	};
	
	struct Input_reader{
		int can_address;

		explicit Input_reader(int);
		Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};
	
	struct Output_applicator{
		int can_address;
		
		explicit Output_applicator(int);
		Output operator()(Robot_outputs);
		Robot_outputs operator()(Robot_outputs,Output)const;
	};	
	
	Output_applicator output_applicator;
	Input_reader input_reader;
	Estimator estimator;
	explicit Tilt(int);
};

#endif
