all:
	g++ -c *.cpp -g  
test:
	g++ -c *.cpp -g
	g++  *.o -g ../../gtest/libgtest.a -pthread
