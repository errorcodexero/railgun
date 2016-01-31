#ifndef TILT_H
#define TILT_H

#include <iostream>
#include "../util/util.h"
#include "../util/interface.h"
#include "../util/driver_station_interface.h"
#include "../util/countdown_timer.h"
#include "../util/quick.h"
#include <set>

struct Tilt{
	class Goal{
		public: 
		enum class Mode{DOWN,GO_TO_ANGLE,STOP,UP};
		
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
	
	class Status_detail{
		public:
		enum class Type{TOP,BOTTOM,MID,ERRORS};
		std::pair<bool,bool> reached_ends;
		bool stalled;
		
		private:
		Status_detail();

		Type type_;
		double angle;
		
		public:
		Type type()const;
		double get_angle()const;
		bool has_ball;		

		static Status_detail top();
		static Status_detail mid(double);
		static Status_detail bottom();	
		static Status_detail error();
	};

	typedef Status_detail Status;

	#define TILT_INPUT(X) X(bool,top) X(bool,bottom) X(bool,has_ball) X(int,ticks) X(double,current)
	struct Input{
		bool top, bottom, has_ball;
		int ticks;
		double current;
	};
	
	struct Input_reader{
		int can_address;

		explicit Input_reader(int);
		Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};
	
	struct Estimator{
		Status_detail last;
		Maybe_inline<int> top,bottom;
		Maybe_inline<double> range()const;

		public: 
		Estimator();
		
		Countdown_timer stall_timer;
		double timer_start_angle;
		
		void update(Time,Input,Output);
		Status_detail get()const;
	};
	
	struct Output_applicator{
		int can_address;
		
		explicit Output_applicator(int);
		Output operator()(Robot_outputs)const;
		Robot_outputs operator()(Robot_outputs,Output)const;
	};	
	
	Output_applicator output_applicator;
	Input_reader input_reader;
	Estimator estimator;
	explicit Tilt(int);
};


std::ostream& operator<<(std::ostream&, Tilt::Status_detail::Type);
std::ostream& operator<<(std::ostream&, Tilt::Goal::Mode);
std::ostream& operator<<(std::ostream&, Tilt::Status_detail);
std::ostream& operator<<(std::ostream&, Tilt::Goal); 
std::ostream& operator<<(std::ostream&, Tilt);

bool operator==(Tilt::Input const&,Tilt::Input const&);
bool operator!=(Tilt::Input const&,Tilt::Input const&);
bool operator<(Tilt::Input const&,Tilt::Input const&);
std::ostream& operator<<(std::ostream&,Tilt::Input const&);

bool operator<(Tilt::Status_detail, Tilt::Status_detail);
bool operator<(Tilt::Status_detail,Tilt::Status_detail);
bool operator==(Tilt::Status_detail, Tilt::Status_detail);
bool operator!=(Tilt::Status_detail, Tilt::Status_detail);

bool operator==(Tilt::Goal,Tilt::Goal);
bool operator!=(Tilt::Goal,Tilt::Goal);
bool operator<(Tilt::Goal,Tilt::Goal);

bool operator==(Tilt::Output_applicator,Tilt::Output_applicator);
bool operator==(Tilt::Input_reader,Tilt::Input_reader);
bool operator==(Tilt::Estimator,Tilt::Estimator);
bool operator!=(Tilt::Estimator,Tilt::Estimator);

bool operator==(Tilt,Tilt);
bool operator!=(Tilt,Tilt);

std::set<Tilt::Input> examples(Tilt::Input*);
std::set<Tilt::Goal> examples(Tilt::Goal*);
std::set<Tilt::Status_detail> examples(Tilt::Status_detail*);
std::set<Tilt::Output> examples(Tilt::Output*);

Tilt::Output control(Tilt::Status_detail, Tilt::Goal);
Tilt::Status status(Tilt::Status_detail);
bool ready(Tilt::Status, Tilt::Goal);

#endif
