TARGET = nano_particles
CXX = g++-9
CXXFLAGS = -O3 -g
CPPFLAGS = -DOMP

all:
	$(CXX) $(CXXFLAGS) *.cpp Utils/*.cpp -o $(TARGET)

openmp:

	$(CXX) -fopenmp $(CXXFLAGS) $(CPPFLAGS)  *.cpp Utils/*.cpp -o $(TARGET)
open:
	code *.cpp Utils/*.cpp Utils/*.h &

check:
	diff outputFile/result.txt backup.txt
run:
	./$(TARGET) > result.txt
