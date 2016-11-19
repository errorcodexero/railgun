#include "shooter_constants.h"
#include "shooter.h"
#include <fstream>

using namespace std;

#define SHOOTER_CONSTANT_ITEMS X(pid.p) X(pid.i) X(pid.d) X(pid.f) X(ground) X(climbed)

Shooter_constants::Shooter_constants():
	ground(GROUND_RPM),climbed(CLIMB_RPM)
{}

ostream& operator<<(ostream& o,Shooter_constants const& a){
	o<<"Shooter_constants( ";
	#define X(NAME) o<<""#NAME<<":"<<a.NAME<<" ";
	SHOOTER_CONSTANT_ITEMS
	#undef X
	return o<<")";
}

bool operator<(Shooter_constants const& a,Shooter_constants const& b){
	#define X(NAME) if(a.NAME<b.NAME) return 1; if(b.NAME<a.NAME) return 0;
	SHOOTER_CONSTANT_ITEMS
	#undef X
	return 0;
}

bool operator==(Shooter_constants const& a,Shooter_constants const& b){
	#define X(NAME) if(a.NAME!=b.NAME) return 0;
	SHOOTER_CONSTANT_ITEMS
	#undef X
	return 1;
}

bool operator!=(Shooter_constants const& a,Shooter_constants const& b){
	return !(a==b);
}

#ifdef SHOOTER_CONSTANTS_TEST
static const char *SHOOTER_CONSTANT_FILE="shooter_constants.txt";
#else
static const char *SHOOTER_CONSTANT_FILE="/home/lvuser/shooter_constants.txt";
#endif

Shooter_constants read_shooter_constants(){
	Shooter_constants r;
	ifstream f(SHOOTER_CONSTANT_FILE);
	string s;
	//note: if file is incomplete/corrupted will just get as much data as it can.
	#define X(NAME) if(f.good()){ getline(f,s); r.NAME=atof(s.c_str()); }
	SHOOTER_CONSTANT_ITEMS
	#undef X
	return r;
}

void write_shooter_constants(Shooter_constants c){
	ofstream f(SHOOTER_CONSTANT_FILE);
	#define X(NAME) if(f.good()) f<<c.NAME<<"\n";
	SHOOTER_CONSTANT_ITEMS
	#undef X
}

#ifdef SHOOTER_CONSTANTS_TEST
Shooter_constants rand(Shooter_constants*){
	Shooter_constants r;
	#define X(NAME) r.NAME=rand()%10;
	SHOOTER_CONSTANT_ITEMS
	#undef X
	return r;
}

void test_shooter_constants_rw(){
	auto a=rand((Shooter_constants*)nullptr);
	write_shooter_constants(a);
	auto b=read_shooter_constants();
	assert(a==b);
}

int main(){
	test_shooter_constants_rw();
}
#endif
