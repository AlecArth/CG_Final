
PLATFORM := $(shell uname)

GL_LIBS = `pkg-config --static --libs glfw3 glew`
EXT =
CPPFLAGS = `pkg-config --cflags glfw3` -std=c++11
# SDL = -lSDL -lSDL_mixer

CC = g++
EXE = game
OBJS = gameMain.o Game.o Shader.o Parser.o

# Any other platform specific libraries here.
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32 -lGL
	EXT = .exe
    DEFS = -DWIN32
endif

ifneq (, $(findstring Darwin, $(PLATFORM)))
    GL_LIBS += -framework OpenGL
    EXT =
endif

.PHONY: all clean


all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS) $(SDL)

gameMain.o: gameMain.cpp
	$(CC) $(CPPFLAGS) -c gameMain.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

Game.o: Game.h Game.cpp
	$(CC) $(CPPFLAGS) -c Game.cpp


Parser.o: Parser.h Parser.cpp
	$(CC) $(CPPFLAGS) -c Parser.cpp

clean:
	rm -f *.o $(EXE)$(EXT)
