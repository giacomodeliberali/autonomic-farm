CXX      := g++
CXXFLAGS := -pthread 

native:
	$(CXX) $(CXXFLAGS) -o nativemapreduce cppnative/main.cpp -std=c++17


seq:
	$(CXX) $(CXXFLAGS) -o seqmapreduce sequential/main.cpp -std=c++17