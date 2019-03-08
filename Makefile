CXX      := g++
CXXFLAGS := -pthread

pipeline: main.cpp
	$(CXX) $(CXXFLAGS) -o pipeline main.cpp