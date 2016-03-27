all:
	g++ -std=c++11 -Wall -o call  -g -I/usr/include/python2.7 main.cpp PythonInterpreter.h AtomicResultList.h -lpython2.7 -pthread
clean:
	rm call
run:
	./call spam foo

foo2:
	./call spam foo2 2 7
test:
	g++ -std=c++11 -Wall -o test  -g -I/usr/include/python2.7 testInterpreter.cpp PythonInterpreter.h -lpython2.7
demo:
	g++ -std=c++11 -Wall -o demo  -g -I/usr/include/python2.7 demoMain.cpp demo.h -lpython2.7

