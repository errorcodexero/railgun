#ifndef SHOOTER_CONSTANTS_H
#define SHOOTER_CONSTANTS_H

#include "../util/interface.h"

struct Shooter_constants{
	PID_values pid;
	float ground,climbed;
	Shooter_constants();
};

bool operator<(Shooter_constants const&,Shooter_constants const&);
bool operator==(Shooter_constants const&,Shooter_constants const&);
bool operator!=(Shooter_constants const&,Shooter_constants const&);
std::ostream& operator<<(std::ostream&,Shooter_constants const&);

Shooter_constants read_shooter_constants();
void write_shooter_constants(Shooter_constants);

#endif
