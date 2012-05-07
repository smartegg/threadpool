all:
	g++ -c *.cpp  
test:
	g++ -c *.cpp
	g++  *.o ../../gtest/libgtest.a -pthread
