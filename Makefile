CC=g++
CFLAGS=-g -Wall
GLFW_LDFLAGS=-lGL -lGLEW -lglfw
OBJDIR=obj

binaries = final

all: $(binaries)

$(OBJDIR):
	mkdir -p $(OBJDIR)

final: $(OBJDIR)/final.o $(OBJDIR)/glshader.o $(OBJDIR)/vector3.o $(OBJDIR)/point3.o
	$(CC) $^ -o $@ $(GLFW_LDFLAGS)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/final.o: final.cpp final.h common/GLShader.h common/stb_image.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c final.cpp -o $@

$(OBJDIR)/glshader.o: common/GLShader.cpp common/GLShader.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c common/GLShader.cpp -o $@

$(OBJDIR)/vector3.o: common/Vector3.cpp common/Vector3.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c common/Vector3.cpp -o $@

$(OBJDIR)/point3.o: common/Point3.cpp common/Point3.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c common/Point3.cpp -o $@

$(OBJDIR)/final.o: final.cpp final.h common/GLShader.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c final.cpp -o $@

clean:
	rm -rf $(OBJDIR) $(binaries)

.PHONY: all clean