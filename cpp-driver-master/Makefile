CXX = g++
CXXFLAGS = -std=c++0x -fPIC -O2
LDFLAGS = -shared
LIBS = -lfastcgi-daemon2 -ljsoncpp -Lbuild -lcassandra -Iinclude

SOURCES = CassandraManager.h CassandraManager.cpp SentimentGet.cpp Sentiment.cpp
OBJECTS = $(SOURCES:.cpp=.o)

TARGET=libsentiment.so

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)
