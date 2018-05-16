
all: Pareto.exe

clean:	
	rm	main.o	Pareto.exe

Pareto.exe: main.o
	g++ -g -o Pareto main.o

main.o:
	g++ -c -g Pareto.cpp