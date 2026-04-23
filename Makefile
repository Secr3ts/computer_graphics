CC=g++
CCFLAGS=-g
LDFLAGS=-lGL -lglut -lGLU

binaries=testgpl triangle transformations

all: ${binaries}

testgpl: testgpl.cpp
	${CC} $< -o $@ ${LDFLAGS}

triangle: triangle.cpp
	${CC} $< -o $@ ${LDFLAGS}

transformations: transformations.cpp
	${CC} ${CCFLAGS} $< -o $@ ${LDFLAGS}

clean:
	rm ${binaries}
	rm *.o