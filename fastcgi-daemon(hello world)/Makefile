CXX = g++
CXXFLAGS = -std=c++0x -fPIC -O2
LDFLAGS = -shared
LIBS = -lfastcgi-daemon2

SOURCES = Sentiment.cpp
OBJECTS = $(SOURCES:.cpp=.o)

TARGET=libsentiment.so

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)
