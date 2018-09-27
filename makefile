
all: Grafo.exe

clean:	
	rm	main.o	Grafo.exe

Graph.exe: main.o
	g++ -g -o Grafo main.o

main.o:
	g++ -c -g Grafo.cpp