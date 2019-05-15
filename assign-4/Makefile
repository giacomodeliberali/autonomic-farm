CXX      := g++
CXXFLAGS := -pthread -std=c++17


native:
	$(CXX) $(CXXFLAGS) -o nativemapreduce cppnative/main.cpp


seq:
	$(CXX) $(CXXFLAGS) -o seqmapreduce sequential/main.cpp


ff:
	$(CXX)  -I ~/Code/personal/unipi/pd-systems/assign-4/fastflow $(CXXFLAGS) -o ffmapreduce fastflow/main.cpp 