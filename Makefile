CC=g++
CFLAGS=-g -Wall
LDFLAGS=-lGL -lglut -lGLU -lGLEW

binaries = testgpl triangle transformations modern

all: $(binaries)

modern: modern.o glshader.o
	$(CC) $^ -o $@ $(LDFLAGS)

testgpl: testgpl.o
	$(CC) $^ -o $@ $(LDFLAGS)

triangle: triangle.o
	$(CC) $^ -o $@ $(LDFLAGS)

transformations: transformations.o
	$(CC) $^ -o $@ $(LDFLAGS)

# compile object files from same-dir sources
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# modern.o depends on modern.h and common headers
modern.o: modern.cpp modern.h common/GLShader.h
	$(CC) $(CFLAGS) -I. -Icommon -c modern.cpp -o modern.o

# glshader.o comes from common/
glshader.o: common/GLShader.cpp common/GLShader.h
	$(CC) $(CFLAGS) -I. -Icommon -c common/GLShader.cpp -o glshader.o

clean:
	rm -f $(binaries) *.o common/*.o

.PHONY: all clean