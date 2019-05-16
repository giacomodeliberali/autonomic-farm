CXX      := g++
CXXFLAGS := -pthread -std=c++17


native:
	$(CXX) $(CXXFLAGS) -o nativemapreduce cppnative/main.cpp


seq:
	$(CXX) $(CXXFLAGS) -o seqmapreduce sequential/main.cpp


ff:
	$(CXX)  -I ~/Code/personal/unipi/pd-systems/assign-4/fastflow $(CXXFLAGS) -o ffmapreduce fastflow/main.cpp 

all:
	make -B native && make -B ff && make -B seq

test:
	./seqmapreduce 100000 123 && ./nativemapreduce 8 100000 123 && ./ffmapreduce 8 100000 123