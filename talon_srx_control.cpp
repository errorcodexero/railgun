#include "talon_srx_control.h"
#include "WPILib.h"

using namespace std;

Talon_srx_control::Talon_srx_control():talon(NULL){}

Talon_srx_control::Talon_srx_control(int CANBusAddress) {
	talon = new CANTalon(CANBusAddress);
}

void Talon_srx_control::set(Talon_srx_output a, bool enabled) {
	/*talon->ChangeControlMode(CANJaguar::kSpeed);
	talon->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	talon->ConfigEncoderCodesPerRev(1);
	talon->SetPID(a.pid.p,a.pid.i,a.pid.d);
	talon->EnableControl();
	talon->SetExpiration(2.0);
	talon->Set(a.speed,SYNC_GROUP);*/
	(void)a;
	(void)enabled;
}
