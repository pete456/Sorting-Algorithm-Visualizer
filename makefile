exelabel = av
sources = main.o

SDL_LIB = -L/usr/local/lib -lSDL2 -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include
CXXFLAGS = -Wall -c  $(SDL_INCLUDE)

$(exelabel) : $(sources)
	gcc $(sources) $(SDL_LIB) -o $(exelabel) -Wall
	rm *.o

