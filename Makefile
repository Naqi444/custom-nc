CXX = g++
CXXFLAGS = -Wall -g -std=c++11
LDFLAGS = -pthread

TARGET = cnc

SOURCES = main.cpp cNetcatTCP.cpp commandHandler.cpp netcatBase.cpp

OBJECTS = $(SOURCES:.cpp=.o)

HEADERS = commandHandler.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

depend: .depend

.depend: $(SOURCES)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ > ./.depend;

include .depend

