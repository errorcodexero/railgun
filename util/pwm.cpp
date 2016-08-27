#include "pwm.h"

//static const unsigned char PWM_MAX=206,PWM_CENTER=128,PWM_MIN=50;//was 56 to make same as WPIlib
//probably want to decouple the deadzones from the rest of this.
Pwm_output pwm_convert(double p){
//going to emulate this so that we don't need to recalibrate stuff.
//	    void SetBounds(INT32 max, INT32 deadbandMax, INT32 center, INT32 deadbandMin, INT32 min);
//    SetBounds(206, 131, 128, 125, 56);
	/*double range=PWM_MAX-PWM_MIN;
	double range_multiplier=range/2;
	double out=range_multiplier*p+PWM_CENTER;
	if(out>125 && out<131) return PWM_CENTER;
	if(out>PWM_MAX) return PWM_MAX;
	if(out<PWM_MIN) return PWM_MIN;
	return (unsigned char)out;*/
	return p;
}

double from_pwm(Pwm_output a){
	/*double range=PWM_MAX-PWM_MIN;
	double range_multiplier=range/2;
	return (double)((int)a-PWM_CENTER)/range_multiplier;*/
	return a;
}

