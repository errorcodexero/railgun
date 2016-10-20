CXX = arm-frc-linux-gnueabi-g++
CXXFLAGS = -std=c++14

SRC = \
	auto_null.cpp\
	auto_reach.cpp\
	auto_static.cpp\
	auto_statictwo.cpp\
	auto_stop.cpp\
	auto_test.cpp\
	barrelracer.cpp\
	cheval.cpp\
	delay.cpp\
	executive.cpp\
	low_bar_low_score.cpp\
	low_bar_wall_high_score.cpp\
	low_bar_wall_low_score.cpp\
	portcullis.cpp\
	step.cpp\
	teleop.cpp\
	test.cpp\
	timed.cpp


OBJS = $(SRC:.cpp=.o)


executive.a: $(OBJS)
	$(AR) r $@ $(OBJS)
