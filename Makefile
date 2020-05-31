CC = gcc
CXX = g++

LIBS = -lglfw -framework OpenGL -framework Cocoa
IDIR = include

CFLAGS = -std=c11 -Wall -I$(IDIR)
CPPFLAGS = -std=c++17 -Wall -Wextra -I$(IDIR)

DEPS = shader.h
OBJ = main.o glad.o stb_image.o

%.o: %.cc $(DEPS)
	$(CXX) -c -o $@ $< $(CPPFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CXX) -o $@ $^ $(CPPFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm main *.o