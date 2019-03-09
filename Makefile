CXX      := g++
CXXFLAGS := -pthread

pipeline:
	$(CXX) $(CXXFLAGS) -o pipeline main.cpp stages.cpp

clean:
	rm pipeline