CXX = g++
CXXFLAGS = -Wall -g

mytest: pqueue.o mytest.cpp
	$(CXX) $(CXXFLAGS) pqueue.o mytest.cpp -o mytest

driver: pqueue.o driver.cpp
	$(CXX) $(CXXFLAGS) pqueue.cpp driver.cpp -o driver

pqueue.o: pqueue.h pqueue.cpp
	$(CXX) $(CXXFLAGS) -c pqueue.cpp

clean:
	rm *.o*
	rm *~ 

runDriver:
	./driver

runMytest:
	./mytest

debugDriver:
	gdb gdbarg1 --args ./driver

debugMytest:
	gdb gdbarg1 --args ./mytest

student:
	cp pqueue.h pqueue.cpp mytest.cpp ~/cs341proj/proj3/
