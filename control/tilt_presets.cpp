#include "tilt_presets.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cassert>
#include <string>

using namespace std;

Tilt_presets::Tilt_presets(){
	top=0;
	level=83;
	low=110;
	cheval=110;
}

#define PRESETS X(top) X(level) X(low) X(cheval)

bool operator<(Tilt_presets const& a,Tilt_presets const& b){
	#define X(NAME) if(a.NAME<b.NAME) return 1; if(b.NAME<a.NAME) return 0;
	PRESETS
	#undef X
	return 0;
}

bool operator==(Tilt_presets const& a,Tilt_presets const& b){
	#define X(NAME) if(a.NAME!=b.NAME) return 0;
	PRESETS
	#undef X
	return 1;
}

bool operator!=(Tilt_presets const& a,Tilt_presets const& b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Tilt_presets const& a){
	return o<<"Presets( top:"<<a.top<<"  level:"<<a.level<<"  low:"<<a.low<<"  cheval:"<<a.cheval<<")";
}

#ifdef TILT_PRESETS_TEST
static const auto PRESET_FILE="presets1.txt";
#else
static const auto PRESET_FILE="/home/lvuser/presets1.txt";
#endif

void write_tilt_presets(Tilt_presets const& a){
	ofstream o(PRESET_FILE);
	if(!o.good()){
		cerr<<"Error: could not open file to write presets\n";
		return;
	}
	#define X(NAME) o<<a.NAME<<"\n";
	PRESETS
	#undef X
}

Tilt_presets read_tilt_presets(){
	Tilt_presets r;
	ifstream f(PRESET_FILE);
	if(!f.good()){
		//if can't open file then just return the defaults
		cerr<<"Error: could not open preset file.  Using defaults.\n";
		return r;
	}
	string s;
	#define X(NAME){ getline(f,s); r.NAME=atof(s.c_str()); }
	PRESETS
	#undef X
	return r;
}

#ifdef TILT_PRESETS_TEST
Tilt_presets rand(Tilt_presets*){
	Tilt_presets r;
	#define X(NAME) r.NAME=rand()%10;
	PRESETS
	#undef X
	return r;
}

void test_preset_rw(){
	auto a=rand((Tilt_presets*)nullptr);
	write_tilt_presets(a);
	auto b=read_tilt_presets();
	assert(a==b);
}

int main(){
	test_preset_rw();
}
#endif
