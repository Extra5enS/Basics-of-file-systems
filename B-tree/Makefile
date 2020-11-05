CC = g++
KEY = -Wall -Werror
NOLINK = -c


all: 
	$(CC) source/b-tree-node.c $(KEY) $(NOLINK)
	$(CC) source/b-tree.c $(KEY) $(NOLINK)

demo: all
	mkdir build -p
	$(CC) *.o demo/main.cpp -o build/demo
	rm *.o

show: demo
	./build/demo

clean:	
	rm *.o -f
	rm -fr build


