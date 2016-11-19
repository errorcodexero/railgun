#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include "countdown_timer.h"

class Debounce{
	#define DEBOUNCE_ITEMS(X) \
		X(Countdown_timer,timer)\
		X(bool,value)

	#define X(A,B) A B;
	DEBOUNCE_ITEMS(X)
	#undef X

	public:
	Debounce();
	bool get()const;
	void update(Time,bool);

	friend bool operator<(Debounce const&,Debounce const&);
	friend bool operator==(Debounce const&,Debounce const&);
	friend std::ostream& operator<<(std::ostream&,Debounce const&);
};

bool operator<(Debounce const&,Debounce const&);
bool operator!=(Debounce const&,Debounce const&);
std::ostream& operator<<(std::ostream&,Debounce const&);

#endif
