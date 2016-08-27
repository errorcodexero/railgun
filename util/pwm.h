#ifndef PWM_H
#define PWM_H

typedef double Pwm_output;

Pwm_output pwm_convert(double);
double from_pwm(Pwm_output);

#endif