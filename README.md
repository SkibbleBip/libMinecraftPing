# libMinecraftPing

A lightweight and minimalist **Server List Ping (SLP) protocol** implementation for Minecraft, written in C++ with C bindings and Rust support.

Query Minecraft servers remotely to retrieve server status, player count, MOTD, and latency information - perfect for server monitoring, web dashboards, and game client integrations.

## Features

- **Multi-language support**: Use from C++, C, or Rust applications
- **Cross-platform**: Supports Windows and Unix-like systems (Linux, macOS)
- **Lightweight**: Minimal dependencies, optimized for performance
- **SRV DNS lookup**: Automatic DNS SRV record resolution for Minecraft servers
- **Error handling**: Comprehensive error codes for debugging
- **Multiple linking options**: Static, shared, and dynamic linking support
- **No exceptions**: C++ code compiled without exception support for minimal overhead

## Architecture

The library consists of:
- **C++ Core** (`main.cpp`): Core SLP protocol implementation with the `Ping` class
- **C Bindings** (`main_c.cpp`): C-compatible wrapper functions for the C++ implementation
- **Rust Support** (`src/minecraft_ping/`): Rust crate with FFI bindings to the C++ library
- **Header API** (`MinecraftPing.h`): Public interface for both C++ and C

## Building

The project uses a Makefile for compilation. Navigate to the `src/` directory and use one of the following commands:

```bash
make static    # Compile static library (.a / .lib)
make shared    # Compile shared library (.so)
make dll       # Compile Windows DLL (Windows only)
make all       # Compile all versions for your OS
make clean     # Remove all build artifacts
```

All compiled libraries are placed in `build/` with subdirectories: `static/`, `shared/`, and `dll/`.

## Linking

### Windows - Static Linking

```bash
g++ main.cpp build/static/libMinecraftPing.a -lwsock32 -liphlpapi -o program.exe
```

### Windows - Dynamic Linking (DLL)

```bash
# Link against import library
g++ main.cpp build/dll/libMinecraftPing.a -o program.exe

# Distribute with DLL
# Place libMinecraftPing.dll in the same directory as program.exe
```

### Linux/Unix - Static Linking

```bash
g++ main.cpp build/static/libMinecraftPing.a -o program
```

### Linux/Unix - Dynamic Linking (Shared Library)

```bash
# Compile and link
g++ main.cpp build/shared/libMinecraftPing.so -o program

# Run with shared library in LD_LIBRARY_PATH
LD_LIBRARY_PATH=./ ./program
```

## Platform Support

| Platform | Status | Static | Shared | DLL |
|----------|--------|--------|--------|-----|
| Windows  | ✓      | ✓      | ✓      | ✓   |
| Linux    | ✓      | ✓      | ✓      | -   |
| macOS    | ✓      | ✓      | ✓      | -   |

## Documentation

For detailed usage examples, API reference, error codes, and advanced features, see the [Wiki](https://github.com/SkibbleBip/libMinecraftPing/wiki).

## License

This project is licensed under the **GNU General Public License v3.0** - see the [LICENSE](LICENSE) file for details.
