build:
	gcc -std=c99 ./src/*.c -lSDL2 -o edgar

run:
	./edgar

clean:
	rm edgar
