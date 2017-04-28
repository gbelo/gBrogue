LIBTCODDIR=libtcod-1.5.2
SDLDIR=SDL-1.2.14
CFLAGS=$(FLAGS) -I$(LIBTCODDIR)/include -I$(SDLDIR)/include -IBrogueCode -IPlatformCode -DBROGUE_TCOD -Wall

%.o : %.c
	gcc $(CFLAGS) -O2 -s -o $@ -c $< 

OBJS=BrogueCode/Architect.o \
	BrogueCode/Combat.o \
	BrogueCode/Dijkstra.o \
	BrogueCode/Globals.o \
	BrogueCode/IO.o \
	BrogueCode/Buttons.o \
	BrogueCode/MainMenu.o \
	BrogueCode/Items.o \
	BrogueCode/Light.o \
	BrogueCode/Monsters.o \
	BrogueCode/Movement.o \
	BrogueCode/RogueMain.o \
	BrogueCode/Random.o \
	BrogueCode/Recordings.o \
	PlatformCode/main.o \
	PlatformCode/platformdependent.o \
	PlatformCode/tcod-platform.o 

all : brogue

brogue-icon.o : brogue-icon.ico icon.rc
	windres icon.rc brogue-icon.o

brogue : ${OBJS} brogue-icon.o 
	g++ -o brogue.exe ${OBJS} brogue-icon.o -L. -ltcod-mingw -lSDL -L$(LIBTCODDIR)/ -static-libgcc -static-libstdc++ -mwindows
