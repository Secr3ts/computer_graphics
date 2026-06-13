CC=g++
CFLAGS=-g -Wall
GLFW_LDFLAGS=-lGL -lGLEW -lglfw
OBJDIR=obj

binaries = final
IMGUI_OBJ = \
	$(OBJDIR)/imgui.o \
	$(OBJDIR)/imgui_demo.o \
	$(OBJDIR)/imgui_draw.o \
	$(OBJDIR)/imgui_tables.o \
	$(OBJDIR)/imgui_widgets.o \
	$(OBJDIR)/imgui_impl_glfw.o \
	$(OBJDIR)/imgui_impl_opengl3.o

all: $(binaries)

$(OBJDIR):
	mkdir -p $(OBJDIR)

final: $(OBJDIR)/final.o $(OBJDIR)/model.o $(OBJDIR)/glshader.o $(OBJDIR)/vector3.o $(OBJDIR)/point3.o $(OBJDIR)/camera.o $(IMGUI_OBJ)
	$(CC) $^ -o $@ $(GLFW_LDFLAGS)

$(OBJDIR)/final.o: final.cpp final.h common/GLShader.h common/stb_image.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c final.cpp -o $@

$(OBJDIR)/glshader.o: common/GLShader.cpp common/GLShader.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c common/GLShader.cpp -o $@

$(OBJDIR)/vector3.o: common/Vector3.cpp common/Vector3.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c common/Vector3.cpp -o $@

$(OBJDIR)/point3.o: common/Point3.cpp common/Point3.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c common/Point3.cpp -o $@

$(OBJDIR)/camera.o: common/camera.cpp common/camera.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c common/camera.cpp -o $@

$(OBJDIR)/model.o: common/model.cpp common/model.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -c common/model.cpp -o $@

$(OBJDIR)/imgui.o: common/imgui/imgui.cpp common/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -Icommon/imgui -Icommon/imgui/backends -c common/imgui/imgui.cpp -o $@

$(OBJDIR)/imgui_draw.o: common/imgui/imgui_draw.cpp common/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -Icommon/imgui -Icommon/imgui/backends -c common/imgui/imgui_draw.cpp -o $@

$(OBJDIR)/imgui_demo.o: common/imgui/imgui_demo.cpp common/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -Icommon/imgui -Icommon/imgui/backends -c common/imgui/imgui_demo.cpp -o $@

$(OBJDIR)/imgui_tables.o: common/imgui/imgui_tables.cpp common/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -Icommon/imgui -Icommon/imgui/backends -c common/imgui/imgui_tables.cpp -o $@

$(OBJDIR)/imgui_widgets.o: common/imgui/imgui_widgets.cpp common/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -Icommon/imgui -Icommon/imgui/backends -c common/imgui/imgui_widgets.cpp -o $@

$(OBJDIR)/imgui_impl_glfw.o: common/imgui/backends/imgui_impl_glfw.cpp common/imgui/backends/imgui_impl_glfw.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -Icommon/imgui -Icommon/imgui/backends -c common/imgui/backends/imgui_impl_glfw.cpp -o $@

$(OBJDIR)/imgui_impl_opengl3.o: common/imgui/backends/imgui_impl_opengl3.cpp common/imgui/backends/imgui_impl_opengl3.h | $(OBJDIR)
	$(CC) $(CFLAGS) -I. -Icommon -Icommon/imgui -Icommon/imgui/backends -c common/imgui/backends/imgui_impl_opengl3.cpp -o $@

clean:
	rm -rf $(OBJDIR) $(binaries)

.PHONY: all clean