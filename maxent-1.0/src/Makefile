all : test
	rm *.o

test : main.o maxent.o
	g++ -o maxent main.o maxent.o

maxent.o : maxent.cpp maxent.h
	g++ -c maxent.cpp -o maxent.o

main.o : main.cpp strid.h varstr.h maxent.h
	g++ -c main.cpp -o main.o

clean : 
	rm maxent
