build:
	gcc -std=c99 ./src/*.c -lSDL2 -o Jelly

run:
	./Jelly

clean:
	rm Jelly
