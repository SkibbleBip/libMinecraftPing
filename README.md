# libMinecraftPing
Lightweight and minimalist Server List Ping (SLP) protocol API library for Minecraft written in C/C++


# Compiling
This project supports both Windows and most \*nix OS's, as well as both C and C++ compilers. These instructions assume you are using the GNU C++ compiler / MINGW C++ compiler.

## Windows
### Static Linking
To compile an object:\
`g++ -c main.cpp -o mcping.o`\
To add it to an archive library: \
`ar rcs libMinecraftPing.a mcping.o`\
To statically link into a project:\
`g++ project.cpp libMinecraftPing.a -lwsock32 -liphlpapi` \
The output executable will be `a.exe` and will contain both the executable binary of the API as well as the linked socket libraries.
### Dynamic Linking
Compile the object: \
`g++ -c main.cpp -o mcping.o` \
Link the needed external libraries and create an archive of the dynamic library: \
`g++ -shared -Wl,--out-implib=libMinecraftPing.a -Wl,--dll mcping.o -o libMinecraftPing.dll -s -lwsock32 -liphlpapi` \
Link the libMinecraftPing.a to the project, and drop the generated libMinecraftPing.dll into the same folder as the project exe. \
This method does not require linking to any other libraries after a one time link with the SLP API: \
`g++ project.cpp libMinecraftPing.a` 

## GNU+Linux
### Static Linking
To compile an object: \
`g++ -c main.cpp -o mcping.o` \
To create static archive: \
`ar rcs libMinecraftPing.a mcping.o` \
Compile the static library into the project executable: \
`g++ project.cpp libMinecraftPing.a` \
The output executable will be `a.out`. No other external libraries will need to be linked on the GNU+Linux System for compiling. 
### Dynamic Linking
To compile a dynamic shared library: \
`g++ -fPIC main.cpp -shared -o libMinecraftPing.so` \
Dynamically link on compilation: \
`g++ project.cpp libMinecraftPing.so` \
Place `libMinecraftPing.so` in the same folder as the project source when compiling, and then keep the shared library in the executable's root folder.
