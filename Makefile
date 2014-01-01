# copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
#

CXX=g++
CXXFLAGS=-Wall -O2

SOURCES= \
 CircularArray.cpp \
 Sliding.cpp \
 SenderSlidingWindow.cpp \
 ReceiverSlidingWindow.cpp \
 RandomNumber.cpp \
 sim.cpp

OBJECTS=$(SOURCES:.cpp=.o)

all: simsw

simsw: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ 

format-code:
	for i in `ls *.cpp`; \
		do \
			uncrustify -c default.cfg --replace $$i; \
		done

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@
