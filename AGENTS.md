# Transcendence Development Guide

## Building

### Prerequisites

- Visual Studio 2022 (Community edition or higher)
- MSBuild (included with VS 2022, located at `C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe`)

### Solution File

The main solution is `Transcendence\Transcendence.sln` (Visual Studio 2022, format version 17).

### Build Configurations

- **Debug|Win32** — Standard debug build
- **Release|Win32** — Standard release build
- **SteamDebug|Win32** / **SteamRelease|Win32** — Steam-specific builds

### Building from Command Line

```
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Transcendence\Transcendence.sln -p:Configuration=Debug -p:Platform=Win32 -m
```

Use `-p:Configuration=Release` for release builds. The `-m` flag enables parallel compilation.

Note: Use `-p:` (dash prefix) rather than `/p:` for switches, as `/` can be misinterpreted in some shell environments.

### Build Output

The compiled executable is written to `Transcendence\Game\Transcendence.exe`.

### Project Structure

The solution contains three main subsystems:

**Alchemy** (`Alchemy\`) — Low-level engine libraries:
- **Kernel** — Core foundation library (no dependencies)
- **CodeChain** — Scripting language engine (depends on Kernel)
- **Graphics** — Graphics abstraction layer (depends on Kernel)
- **IntelJPEGUtil** — JPEG image support (depends on Kernel)
- **DirectXUtil** — DirectX rendering utilities (depends on Graphics, IntelJPEGUtil, Kernel)
- **XMLUtil** — XML parsing (depends on Kernel)
- **zlibstat** — zlib compression (static library, no internal dependencies)

**Mammoth** (`Mammoth\`) — Game engine libraries:
- **TSE** (Transcendence Space Engine) — Core game engine (depends on all Alchemy libraries)
- **TSUI** — Game UI framework (depends on TSE and Alchemy libraries)

**Transcendence** (`Transcendence\`) — Game application:
- **TransAPI** — API library
- **SteamUtil** — Steam platform integration
- **Transcendence** — Main game executable (depends on all of the above)

The following projects exist in the solution but are not built in Debug|Win32:
- TransCore
- TransWorkshop
- LibJPEGTurboUtil
