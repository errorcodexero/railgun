#ifndef TILT_H
#define TILT_H

#include <iostream>
#include "../util/util.h"
#include "../util/interface.h"
#include "../util/driver_station_interface.h"
#include "../util/countdown_timer.h"
#include "../util/quick.h"
#include <set>
#include <string>

struct Tilt{
	class Goal{
		public: 
		#define TILT_GOAL_MODES \
			X(DOWN) \
			X(STOP) \
			X(UP) \
			X(GO_TO_ANGLE)
		#define X(name) name,
		enum class Mode{TILT_GOAL_MODES};	
		#undef X
	
		private:
		Goal();

		Mode mode_;
		double angle_min,angle_target,angle_max;//in degrees from top
		
		public:
		Mode mode()const;
		std::array<double,3> angle()const;
		
		static Goal up();
		static Goal down();
		static Goal low();
		static Goal level();
		static Goal go_to_angle(std::array<double,3>);
		static Goal stop();
		
		bool learn_bottom;
	};
	
	typedef double Output;	
	
	class Status_detail{
		public:
		#define TILT_STATUS_DETAIL_TYPES \
			X(TOP) \
			X(MID) \
			X(BOTTOM) \
			X(ERRORS)
		#define X(name) name, 
		enum class Type{TILT_STATUS_DETAIL_TYPES};
		#undef X
		std::pair<bool,bool> reached_ends;
		bool stalled;
		
		private:
		Status_detail();

		Type type_;
		double angle;//in degrees from top
		float pot_value_;//in volts from pot
		
		public:
		Type type()const;
		double get_angle()const;
		float pot_value()const;
	
		static Status_detail top();
		static Status_detail mid(double,double);
		static Status_detail bottom(double,double);	
		static Status_detail error();
	};

	struct Status{
		typedef Status_detail::Type Type;
		Type type;
		double angle;//in degrees
		Status(Tilt::Status::Type,double);
	};

	struct Input{
		float pot_value;
		double current;
		bool top;
	};
	
	struct Input_reader{
		Input operator()(Robot_inputs)const;
		Robot_inputs operator()(Robot_inputs,Input)const;
	};
	
	struct Estimator{
		Status_detail last;

		public: 
		Estimator();
		
		Countdown_timer stall_timer;
		double timer_start_angle;
		
		void update(Time,Input,Output);
		Status_detail get()const;
	};
	
	struct Output_applicator{
		Output operator()(Robot_outputs)const;
		Robot_outputs operator()(Robot_outputs,Output)const;
	};	
	
	Output_applicator output_applicator;
	Input_reader input_reader;
	Estimator estimator;
};

std::ostream& operator<<(std::ostream&, Tilt::Status);
std::ostream& operator<<(std::ostream&, Tilt::Status_detail::Type);
std::ostream& operator<<(std::ostream&, Tilt::Goal::Mode);
std::ostream& operator<<(std::ostream&, Tilt::Status_detail);
std::ostream& operator<<(std::ostream&, Tilt::Goal); 
std::ostream& operator<<(std::ostream&, Tilt);

bool operator==(Tilt::Input const&,Tilt::Input const&);
bool operator!=(Tilt::Input const&,Tilt::Input const&);
bool operator<(Tilt::Input const&,Tilt::Input const&);
std::ostream& operator<<(std::ostream&,Tilt::Input const&);

bool operator<(Tilt::Status,Tilt::Status);
bool operator==(Tilt::Status,Tilt::Status);
bool operator!=(Tilt::Status,Tilt::Status);

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
std::set<Tilt::Status> examples(Tilt::Status*);
std::set<Tilt::Output> examples(Tilt::Output*);

Tilt::Output control(Tilt::Status_detail, Tilt::Goal);
Tilt::Status status(Tilt::Status_detail);
bool ready(Tilt::Status, Tilt::Goal);

std::array<double,3> make_tolerances(double);

void update_positions();
void tilt_learn(float value,std::string const&);

#endif
