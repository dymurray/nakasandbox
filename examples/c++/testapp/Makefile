# Simple makefile for a simple SDK test application.

# Change this to point to where the SDK was installed
NCHAINDIR=		../../..

CC=				g++

OBJS=			testapp.o
APPNAME=		testapp

BOOSTLIBS=      -lboost_system -lboost_thread

CRYPTOLIBS=		-lcryptopp
CRYPTOINCLUDES=	-I/usr/include/cryptopp

#NCHAINLIBS=		$(NCHAINDIR)/lib/libnakasendo_shared.so
NCHAINLIBS=		$(NCHAINDIR)/lib/libnakasendo.a
NCHAININCLUDES=	-I$(NCHAINDIR)/include

#INCLUDES=		-I. $(CRYPTOINCLUDES) $(NCHAININCLUDES)
#CCFLAGS=		$(INCLUDES)
LNFLAGS=		-Wl,-rpath,$(NCHAINDIR)/lib
LIBS=			$(CRYPTOLIBS) $(NCHAINLIBS) $(BOOSTLIBS)

# Dependencies
GLOBALDEPS=		Makefile
DEPENDS=		$(patsubst %.o,%.d,$(OBJS))

#############
# Build rules
#############

# Top level
.PHONY:	all
all:	depend $(APPNAME)

# Dependencies
include $(DEPENDS)

%.d:	%.cpp
		@echo -- makedepend $@
		@bash -c '$(CC) $(CCFLAGS) -M $(<F) >$(*F).d; \
			[ -s $(*F).d ] || rm -f $(*F).d '; \
			exit_status=$$? ; \
		if [ $${exit_status} -ne 0 ]; then exit $${exit_status}; fi

.PHONY:	depend
depend:	$(DEPENDS)

# Application
$(APPNAME):	$(OBJS)
	$(CC) $(LNFLAGS) -o $(APPNAME) $(OBJS) $(LIBS)

%.o:	%.cpp $(GLOBALDEPS)
	$(CC) -c $(CCFLAGS) $*.cpp

####################
# Other useful rules
####################

.PHONY:	clean
clean:
	rm -f $(APPNAME) *.o *.d
