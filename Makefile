CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g
LDFLAGS=-g
LDLIBS=

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

