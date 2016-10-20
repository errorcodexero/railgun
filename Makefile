#
# Makefile for creating
#

WPILIB=/usr/local/wpilib

all: sabre

sabre::
	(cd util ; make)
	(cd modes ; make)
	(cd input ; make)
	(cd control ; make)
	(cd roborio ; make WPILIB=$(WPILIB))

clean::
	rm */*.o */*.a

