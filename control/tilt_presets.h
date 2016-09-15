#ifndef TILT_PRESETS_H
#define TILT_PRESETS_H

struct Tilt_presets{
	#define TILT_PRESETS_ITEMS X(top) X(level) X(low) X(cheval)
	double top, level, low, cheval;//angles (in degrees) that it will go to when set to the tilt goals
	Tilt_presets();
};

bool operator!=(Tilt_presets const&,Tilt_presets const&);

void write_tilt_presets(Tilt_presets const& a);
Tilt_presets read_tilt_presets();

#endif
