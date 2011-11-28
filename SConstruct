#
# This file defines how to build PyMite using Scons (www.scons.org)
# Usage:
#   scons tags
#   scons docs
#   scons PLATFORM=<platform>
#

EnsurePythonVersion(2, 6)

import os, string

DEFAULT_PLATFORM = "posix64"

valid_platforms = [os.path.split(x)[1] for x in map(str, Glob("src/platform/*"))
                   if x not in ("_unmaintained", "COPYING")]

vars = Variables()


# BUILD TARGETS
if "tags" in COMMAND_LINE_TARGETS or "TAGS" in COMMAND_LINE_TARGETS:
    vmfiles = Glob("src/vm/*.c") + Glob("src/vm/*.h")
    ctags = Command('tags', vmfiles, "ctags $SOURCES")
    cscope = Command('cscope.out', vmfiles, "cscope -b -c -R $SOURCES")
    pyc_tools = Command('src/tools/cscope.out', Glob("src/tools/*.py"),
                        "src/tools/pycscope.py -f $TARGET $SOURCE")
    pyc_lib = Command('src/lib/cscope.out', Glob("src/lib/*.py"),
                      "src/tools/pycscope.py -f $TARGET $SOURCE")
    tags = Alias('tags', [ctags, cscope, pyc_tools, pyc_lib])
    Alias('TAGS', tags)


#elif "docs" in COMMAND_LINE_TARGETS or "html" in COMMAND_LINE_TARGETS:
#    srcpath = os.path.join("docs", "src")
#    rstfiles = Glob(os.path.join(srcpath, "*.txt"))
#    htmlpath = os.path.join("docs", "html")
#    Mkdir(htmlpath)
#    htmlfiles = [string.replace(string.replace(str(s), ".txt", ".html", 1), srcpath, htmlpath, 1)
#                 for s in rstfiles]
#    html = [Command(htmlfiles[i], rstfiles[i], "rst2html.py $SOURCE $TARGET")
#            for i in range(len(rstfiles))]
#    htmlalias = Alias("html", html)
#    Alias("docs", htmlalias)


#elif "dist" in COMMAND_LINE_TARGETS:
#    assert "PM_RELEASE" in vars.args.keys(), "Must define PM_RELEASE=RR"
#    dist = Command("pymite-%s.tar.gz" % vars.args["PM_RELEASE"], None,
#                   "src/tools/pmDist.py %s" % vars.args["PM_RELEASE"])
#    AlwaysBuild(dist)
#    Alias("dist", dist)


elif "ipm" in COMMAND_LINE_TARGETS:
    print "Build for your target platform, then run src/tools/ipm.py"
    print "with arguments to connect to the target."
    exit(0)


elif "check" in COMMAND_LINE_TARGETS:
    sconscript_path = os.path.join("src", "tests", "system", "SConscript")
    build_path = os.path.join("src", "tests", "system", "build")
    run_tests = SConscript(sconscript_path, "vars", variant_dir=build_path)
    Clean("check", build_path)


# Build the default platform
else:
    if len(vars.args) == 0:
        vars.args["PLATFORM"] = DEFAULT_PLATFORM
    else:
        if vars.args["PLATFORM"] not in valid_platforms:
            print "Error: must define PLATFORM=<plat> where <plat> is from %s" \
                % str(valid_platforms)
            Exit(1)
    platform_path = os.path.join("src", "platform", vars.args["PLATFORM"])
    sconscript_path = os.path.join(platform_path, "SConscript")
    build_path = os.path.join(platform_path, "build")
    main = SConscript(sconscript_path, "vars", variant_dir=build_path)
    Clean(main, build_path)

#:mode=python: