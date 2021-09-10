# libMinecraftPing
Lightweight and minimalist Server List Ping (SLP) protocol API library for Minecraft written in C++


# Compiling
This project supports both Windows and most \*nix OS's, and supports being linked to C++ and C programs. These instructions assume you are using the GNU C++ compiler / MINGW C++ compiler. \
Compiling is done through the Makefile located in the /src folder. 3 options are available for compiling: a static library, a shared object, and a Dynamic Link Library (Windows only)\
The commands to compile are as follows: \

`make static`	- Compile static library \
`make shared`	- Compile shared library \
`make dll`	- Compile DLL file for windows \
`make all`	- Compile all versions for the current operating system \
`make clean`	- Clean all compiled data \

All compiled data will be stored in their respective subfolders located in build/ \


## Windows
### Static Linking
To compile:\
`make static` \
To statically link into a project:\
`g++ project.cpp libMinecraftPing.a -lwsock32 -liphlpapi` \
The output executable will be `a.exe` and will contain both the executable binary of the API as well as the linked socket libraries.
### Dynamic Linking
Compile the Dynamic Link Library: \
`make dll` \
Link the libMinecraftPing.a to the project, and drop the generated libMinecraftPing.dll into the same folder as the project exe. \
This method does not require linking to any other libraries after a one time link with the SLP API: \
`g++ project.cpp libMinecraftPing.a` \
If you would rather use a shared object when compiling, you can do: \
`make shared` \
Then when compiling your project: \
`g++ project.cpp libMinecraftPing.so` \
Ship your executable with `libMinecraftPing.so` in the root directory with the executable.

## GNU+Linux
### Static Linking
Compile the static library: \
`make static` \
Compile the static library into the project executable: \
`g++ project.cpp libMinecraftPing.a` \
The output executable will be `a.out`. No other external libraries will need to be linked on the GNU+Linux System for compiling. 
### Dynamic Linking
Compile the library: \
`make shared` \
Dynamically link on compilation: \
`g++ project.cpp libMinecraftPing.so` \
Place `libMinecraftPing.so` in the same folder as the project source when compiling, and then keep the shared library in the executable's root folder.
# Wiki
For more information, check the docs in the [Wiki:](https://github.com/SkibbleBip/libMinecraftPing/wiki).
