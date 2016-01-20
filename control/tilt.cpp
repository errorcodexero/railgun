#include "tilt.h"

Tilt::Output_applicator::Output_applicator(int a):can_address(a){
	assert(can_address>=0 && (unsigned)can_address<Robot_outputs::TALON_SRX_OUTPUTS);
}

#ifdef TILT_TEST
#include "formal.h"

int main(){
	Tilt a(1);
	tester(a);	
}
#endif

