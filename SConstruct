#!/usr/bin/env python

import os

env = SConscript("../../godot-cpp/SConstruct")

env.Append(CPPPATH=["src", "thirdparty"])

if env["platform"] == "linux":
    env.Append(LIBS=["dtc"])
    env.Append(LIBPATH=[env.Dir("bin").abspath, "/usr/lib64", "/usr/lib"])
    env.Append(LINKFLAGS=[
        "-Wl,-rpath,$ORIGIN",
        "-L" + env.Dir("bin").abspath,
        "-L/usr/lib64",
        "-L/usr/lib",
    ])
elif env["platform"] == "windows":
    env.Append(LIBPATH=[env.Dir("bin").abspath])
    if env.get("use_mingw", False):
        dtc_dll = env.File("bin/dtc.dll").abspath
        if os.path.isfile(dtc_dll):
            env.Append(LINKFLAGS=["-l:dtc.dll"])
        else:
            env.Append(LINKFLAGS=["-l:libdtc.dll"])
    else:
        env.Append(LIBS=["dtc"])

sources = Glob("src/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "bin/libdectalk_mini.{}.{}.framework/libdectalk_mini.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "bin/libdectalk_mini.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "bin/libdectalk_mini.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "bin/libdectalk_mini{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

env.NoCache(library)
Default(library)
