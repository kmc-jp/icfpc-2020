SRCS:=$(wildcard *.cpp)
OBJS:=$(SRCS:%.cpp=%.o)
DEPS:=$(SRCS:%.cpp=%.d)
CXXFLAGS:=-std=c++17 -Og -g -Wall -Wextra

all: interpret

-include $(DEPS)

interpret: $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^

%.o:%.cpp
	$(CXX) -c $(CXXFLAGS) -MMD -MP $<

.PHONY: clean
clean:
	-rm $(OBJS) $(DEPS) interpret
