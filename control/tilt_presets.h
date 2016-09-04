#ifndef TILT_PRESETS_H
#define TILT_PRESETS_H

struct Tilt_presets{
	double top, level, low, cheval;//angles (in degrees) that it will go to when set to the tilt goals
	Tilt_presets();
};

void write_tilt_presets(Tilt_presets const& a);
Tilt_presets read_tilt_presets();

#endif
