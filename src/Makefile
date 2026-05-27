OBJS	= obj/main.o obj/main_c.o
SOURCE	= main.cpp, main_c.cpp,
HEADER	= MinecraftPing.h
OUT	= libMinecraftPing
CC	= g++
FLAGS	= -s -O3 -Wall -fno-exceptions -fPIC
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

ifeq ($(OS),Windows_NT)
    MKDIR = if not exist "$(1)" mkdir "$(1)"
    RM = rmdir /s /q
else
    MKDIR = mkdir -p $(1)
    RM = rm -rf
endif


static: $(OBJS)
	$(call MKDIR,$(STATIC))
	ar rcs $(STATIC)/$(OUT).a $(OBJS)

shared: $(OBJS)
	$(call MKDIR,$(SHARED))
ifeq ($(OS),Windows_NT)
		$(CC) -fPIC $(SOURCE) -shared -o $(SHARED)/$(OUT).so -lwsock32 -liphlpapi
else
		$(CC) -fPIC -shared -s $(OBJS) -o $(SHARED)/$(OUT).so
endif

dll: $(OBJS)
	$(call MKDIR,$(DLL))
	$(CC) -shared -Wl, --out-implib=$(DLL)/$(OUT).a -Wl,--dll $(OBJS) -o $(OUT) -s -lwsock32 -liphlpapi



libMinecraftPing: $(OBJS)
	$(CC) -s $(OBJS) -o $(OUT)
obj/main.o: main.cpp
	$(call MKDIR,$(OBJ))
	$(CC) $(FLAGS) -c main.cpp -o $(OBJ)/main.o

obj/main_c.o: main_c.cpp
	$(call MKDIR,$(OBJ))
	$(CC) $(FLAGS) -c main_c.cpp -o $(OBJ)/main_c.o


clean:
	-$(RM) $(OBJ)
	-$(RM) $(STATIC)
	-$(RM) $(SHARED)
	-$(RM) $(DLL)
