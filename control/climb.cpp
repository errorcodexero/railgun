#include "climb.h"
#include <stdlib.h>

std::ostream& operator<<(std::ostream& o,Climb::Goal a){
	#define X(name) if(a==Climb::Goal::name)return o<<"Climb::Goal("#name")";
	X(UP) X(STOP) X(DOWN)
	#undef X
	assert(0);
}

std::ostream& operator<<(std::ostream& o,Climb::Input){
	return o<<"Climb::Input()";
}

std::ostream& operator<<(std::ostream& o,Climb::Status_detail){
	return o<<"Climb::Status_detail()";
}

std::ostream& operator<<(std::ostream& o,Climb){
	return o<<"Climb()";
}

bool operator==(Climb::Input,Climb::Input){ return true; }
bool operator==(Climb::Status_detail,Climb::Status_detail){ return true; }
bool operator<(Climb::Status_detail,Climb::Status_detail){ return false; }

std::set<Climb::Input> examples(Climb::Input*){
	return {};
}

std::set<Climb::Goal> examples(Climb::Goal*){
	return {Climb::Goal::UP,Climb::Goal::STOP,Climb::Goal::DOWN};
}

std::set<Climb::Status_detail> examples(Climb::Status_detail*){
	return {};
}

Climb::Output control(Climb::Status_detail,Climb::Goal goal){
	#define X(name) if(goal==Climb::Goal::name)return Climb::Output::name;
	X(UP) X(STOP) X(DOWN)
	#undef X
	assert(0);
}

Climb::Status status(Climb::Status_detail a){
	return a;
}

bool ready(Climb::Status,Climb::Goal){
	return true;
}

#ifdef CLIMB_TEST
#include "formal.h"

int main(){
	Climb a;
	tester(a);
}
#endif
