INC_DIR=/usr/local/Cellar/boost/1.56.0/include

CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g -c -Wall $(addprefix -I,$(INC_DIR))
LDFLAGS=-g
LDLIBS=


CCFLAGS=

SRCS=$(shell printf "%s " *.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: ncredundancy
	
%.o: %.cpp
	$(CXX) $(CPPFLAGS) -I. -MMD -MP -o $@ -c $<
	
-include $(subst .o,.d,$(OBJS))
	
ncredundancy: $(OBJS)
	$(CXX) $(LDFLAGS) -o ncredundancy $(OBJS) $(LDLIBS) 
	
clean:
	$(RM) *.o *.d

dist-clean: clean
	$(RM) ncredundancy
	$(RM) *~

