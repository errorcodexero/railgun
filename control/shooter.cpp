#include "shooter.h"
#include "stdlib.h"

std::ostream& operator<<(std::ostream& o,Shooter::Goal){ return o<<"Shooter::Goal{}"; }
std::ostream& operator<<(std::ostream& o,Shooter::Input){ return o<<"Shooter::Input{}"; }
std::ostream& operator<<(std::ostream& o,Shooter::Status_detail){ return o<<"Shooter::Status_detail"; }
std::ostream& operator<<(std::ostream& o,Shooter){ return o<<"Shooter{}"; }

bool operator==(Shooter::Input,Shooter::Input){ return true; }
bool operator!=(Shooter::Input,Shooter::Input){ return false; }
bool operator<(Shooter::Input,Shooter::Input){ return false; }

bool operator<(Shooter::Status_detail,Shooter::Status_detail){ return false; }
bool operator==(Shooter::Status_detail,Shooter::Status_detail){ return true; }
bool operator!=(Shooter::Status_detail,Shooter::Status_detail){ return false; }

bool operator==(Shooter::Input_reader,Shooter::Input_reader){ return true; }
bool operator<(Shooter::Input_reader,Shooter::Input_reader){ return false; }

bool operator==(Shooter::Estimator,Shooter::Estimator){ return true; }
bool operator!=(Shooter::Estimator,Shooter::Estimator){ return false; }

bool operator==(Shooter::Output_applicator,Shooter::Output_applicator){ return true; }

bool operator==(Shooter a,Shooter b){ return (a.input_reader==b.input_reader && a.estimator==b.estimator && a.output_applicator==b.output_applicator); }
bool operator!=(Shooter a,Shooter b){ return !(a==b); }

Shooter::Input Shooter::Input_reader::operator()(Robot_inputs)const{ return {}; }
Robot_inputs Shooter::Input_reader::operator()(Robot_inputs r,Shooter::Input)const{ return r; }

Shooter::Output Shooter::Output_applicator::operator()(Robot_outputs)const{ return {}; }
Robot_outputs Shooter::Output_applicator::operator()(Robot_outputs r,Shooter::Output)const{ return r; }

Shooter::Status_detail Shooter::Estimator::get()const{ return {}; }
void Shooter::Estimator::update(Time,Shooter::Input,Shooter::Output){} 

std::set<Shooter::Input> examples(Shooter::Input*){ return {{}}; }
std::set<Shooter::Goal> examples(Shooter::Goal*){ return {{}}; }
std::set<Shooter::Status_detail> examples(Shooter::Status_detail*){ return {{}}; }

Shooter::Output control(Shooter::Status_detail,Shooter::Goal){ return {}; }
Shooter::Status status(Shooter::Status_detail a){ return a; }
bool ready(Shooter::Status,Shooter::Goal){ return true; }



#ifdef SHOOTER_TEST
#include "formal.h"

int main(){
	//Shooter a;
	//tester(a);
}

#endif

