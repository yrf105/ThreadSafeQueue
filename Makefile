main: main.cc ThreadSafeQueue.h
	g++ -g main.cc ThreadSafeQueue.h -o  main -lpthread

clean:
	rm -rf *.o main