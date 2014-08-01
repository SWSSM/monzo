all:
	g++ monzo.cpp Copters.cpp Copters_func.cpp -o Mon -lpthread `pkg-config --cflags --libs opencv`

dump:
	g++ monzo.cpp Copters.cpp Copters_func.cpp -g -o   Mon -lpthread `pkg-config --cflags --libs opencv`

clean:
	rm Mon
