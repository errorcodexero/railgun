#
# Makefile for building the SABRE repository.  This is used by jenkins to
# do periodic builds
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

