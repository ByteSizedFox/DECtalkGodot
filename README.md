# DECtalk Mini GDExtension

## Prerequisites

- Place the DECtalk runtime next to the extension binaries.
Linux: `addons/dectalk_mini/bin/libdtc.so`
Windows: `addons/dectalk_mini/bin/libdtc.dll`

## Build (Linux)

Editor build:

```bash
cd addons/dectalk_mini
PATH=/home/user/.local/bin:/usr/local/bin:/usr/bin:/bin CC=/usr/bin/gcc CXX=/usr/bin/g++ \
  scons platform=linux target=editor use_static_cpp=no -j4
```

Game template build:

```bash
cd addons/dectalk_mini
PATH=/home/user/.local/bin:/usr/local/bin:/usr/bin:/bin CC=/usr/bin/gcc CXX=/usr/bin/g++ \
  scons platform=linux target=template_debug use_static_cpp=no -j4
```

## Build (Windows)

Editor build (MinGW):

```bash
cd addons/dectalk_mini
scons platform=windows target=editor use_mingw=yes use_static_cpp=no -j4
```

Game template build (MinGW):

```bash
cd addons/dectalk_mini
scons platform=windows target=template_debug use_mingw=yes use_static_cpp=no -j4
```

Notes:
- On Windows, the runtime DLL is loaded as `dtc.dll`. If your vendor DLL is
  named `libdtc.dll`, keep a copy named `dtc.dll` in the same `bin` folder.
- MinGW builds require the runtime DLLs next to the extension binary:
  `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, and `libwinpthread-1.dll`.
- If you build with MSVC instead of MinGW, you will need an import library named
  `dtc.lib` in `addons/dectalk_mini/bin` (or another LIBPATH) that matches
  `libdtc.dll`.

Licenses:
- See `addons/dectalk_mini/licenses/THIRD_PARTY_NOTICES.txt`.

usage example:
```
extends Node2D

var dectalk = DECtalkMini.new()

func _callback(iwave, length, phoneme) -> void:
	print(iwave) # waveform comes out here in 6.4ms callback chunks

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	dectalk.init(_callback)
	dectalk.start("Hello World", DECtalkMini.WAVE_FORMAT_1M16)
	dectalk.sync() # force audio out here

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
```
