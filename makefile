build:
	gcc -std=c99 ./src/*.c -lSDL2 -o corridor;

run:
	./corridor;

clean:
	rm corridor;
