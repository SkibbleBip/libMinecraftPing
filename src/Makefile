OBJS	= obj/main.o obj/main_c.o
SOURCE	= main.cpp, main_c.cpp,
HEADER	= MinecraftPing.h
OUT	= libMinecraftPing
CC	= g++
FLAGS	= -s -O3 -Wall -fno-exceptions
LFLAGS	=
OBJ	= obj
STATIC  = build/static
SHARED	= build/shared
DLL	= build/dll



all: static shared
	#$(CC) -s -c $(OBJS) -o $(OUT) $(LFLAGS)
ifeq ($(OS),Windows_NT)
		dll
endif



static: $(OBJS)
	mkdir -p $(STATIC)
	ar rcs $(STATIC)/$(OUT).a $(OBJS)

shared: $(OBJS)
	mkdir -p $(SHARED)
ifeq ($(OS),Windows_NT)
		$(CC) -fPIC $(SOURCE) -shared -o $(SHARED)/$(OUT).so -lwsock32 -liphlpapi
else
		$(CC) -shared -s $(OBJS) -o $(SHARED)/$(OUT).so
endif

dll: $(OBJS)
	mkdir -p $(DLL)
	$(CC) -shared -Wl, --out-implib=$(DLL)/$(OUT).a -Wl,--dll $(OBJS) -o $(OUT) -s -lwsock32 -liphlpapi



libMinecraftPing: $(OBJS)
	$(CC) -s $(OBJS) -o $(OUT)
obj/main.o: main.cpp
	mkdir -p $(OBJ)
	$(CC) $(FLAGS) -c main.cpp -o $(OBJ)/main.o

obj/main_c.o: main_c.cpp
	mkdir -p $(OBJ)
	$(CC) $(FLAGS) -c main_c.cpp -o $(OBJ)/main_c.o


clean:
	rm -r -f $(OBJ) $(OUT) $(STATIC) $(SHARED) $(DLL)

