===========================
Platform Marshalled Modules
===========================
Adding Modules to P14p After Compile-Time
=========================================


:Author: Dean Hall


Introduction
------------

P14p has always provided the ability to create Python modules and import them in
your program, provided the modules are added to the P14p binary at compile-time.
What's been missing is the ability to dynamically add and use modules after
compile-time.  Platform Marshalled Modules (PMM) is a new feature that allows
the ability to import a marshalled module in a platform-specific fashion.  This
new feature provides the following benefits:

#. A platform may store marshalled modules on nearly *any* form of information
   storage media.  This includes SEEPROM, auxiliary Flash areas, SD Cards, file
   systems or even network-accessible storage.
#. Compile p14p once and put your main program in a PMM so it can be easily
   modified.  For your platform, you could establish a convention such as the
   platform's main.py contains simply ``import autoexec``.  Then you write your
   application in autoexec.py, convert it to autoexec.pmm and store the
   resulting bytes in your platform's storage area.
#. Override built-in modules.  If you've compiled P14p and distributed it in an
   embedded system and later find a bug in one of the built-in modules, you can
   install a corrected version of that module in the platform storage area.
   P14p will search the platform storage area first by default and use the
   corrected module from there.

The ``posix64`` platform has an example of PMM that uses the platform's file
system as the storage area.  So the marshalled modules are stored as simple
files with the ``.pmm`` file extension.  Using the ``.pmm`` file extension is
the recommended convention for any platform that has a file system.

The rest of this document explains the implementation of PMM and how to use it.


Using PMM
---------

There are two pieces to using PMM in P14p.  First, the port of P14p to a
specific platform *must* implement and register a platform-specific function
that fetches the PMM data and turns it into a Module in RAM.  Second, the
end-user programmer must use the converter tool, ``src/tools/pmPyToPmm.py``, to
marshal his module into a PMM byte stream and store that bytestream in a manner
that allows the platform to search for it by knowing only the module's name.
For example, if the module ``telemetry.py`` is written, the program that uses
the telemetry module has a line of code such as ``import telemetry`` and so
P14p's VM will only have the string ``"telemetry"`` to use when looking up the
module in the platform's storage area.  Now, let's dig into the two pieces to
using PMM in detail.

Implement the Platform Loading Function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

With the introduction of PMM, P14p's platform porting layer has one new API::

    PmReturn_t plat_loadCodeObject(pPmObj_t pname, pPmObj_t *r_cob);

:Aside: The function is called "plat_loadCodeObject" because inside the P14p VM,
        all the information from a compiled Python module is held in a structure
        called a Code Object.  A Code Object structure can be located in a
        microcontroller's flash memory or RAM.  When the P14p VM needs to import
        a module, the VM finds a code object of the matching name and "imports"
        it using ``mod_new`` which effectively turns the code object into a
        module.  Further aside: In Python a code object can hold the
        representation of a module, a class or a function.  Likewise, in P14p,
        the C structure PmFunc_t is used to hold the contents of a function,
        class or module object.

This function is OPTIONAL.  Porting P14p to a new platform does not require that
this function exists.  However, if you want to add the PMM feature to your
platform, you need to implement this function and register it during
``pm_init()``.  Let's look at what ``plat_loadCodeObject()`` has to do.

First, we can see from the function's arguments that it expects a name and
returns by reference a cob, or Code Object.  The function also returns a
PmReturn_t status.  This direct return value should be ``PM_RET_OK`` if the
module was found and loaded into RAM, ``PM_RET_NO`` if the module was not found,
or one of the ``PM_RET_EX_*`` values defined in ``pm.h``, which is how an
exception is reported to the VM.

Inside of ``plat_loadCodeObject()`` the following actions must be performed:

#. Search the platform storage area to see if a PMM with the matching name is
   found.  If a matching PMM is not found, return ``PM_RET_NO``.  In the posix64
   platform example, I search the current working directory for a file with a
   matching name (and .pmm extension).
#. If the platform storage area is NOT in a directly addressable memory  (RAM or
   Flash is directly addressable; SEEPROM or a file is not), the PMM's bytes
   must be copied into RAM so they can be read by P14p's Marshal function.  In
   the posix64 platform example, I allocate a chunk of memory and copy the PMM
   file's contents into it.  Note that P14p memory chunks are limited to 2040
   bytes (as defined by ``HEAP_MAX_LIVE_CHUNK_SIZE``) at the time of this
   writing, so this limits the size of the ``.pmm`` file on the posix64
   platform.
#. Call ``marshal_load()`` and give it the address to the start of the PMM
   bytes.  ``marshal_load()`` will return a code object by reference if the PMM
   file described a module.
#. Check for any errors and report them (as exceptions) if necessary.
   Otherwise, return the code object by reference and return ``PM_RET_OK``.  The
   VM will take care of everything else.

Register the Platform Loading Function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Great, you've made ``plat_loadCodeObject()``, now what?  Well, the VM doesn't
know about your function yet.  You have to explicitly register your function.
In the posix64 example, I perform registration in ``plat_init()``.  I cannot
think of a reason to perform this registration elsewhere, so I highly recommend
it be done there.  Here is the code to do it::

    mod_setPlatLoadCodeObjectFunctionPointer(plat_loadCodeObject);

As you can see, the registration function is part of ``mod.c`` because that's
where ``mod_import()``, the function that calls your ``plat_loadCodeObject()``,
resides.  The registration function is expecting the function pointer as its
only argument.  As long as your load function matches the signature of
``plat_loadCodeObject`` as defined in ``src/vm/plat_interface.h`` everything
should work fine.


Creating a PMM
--------------

I can't call this section "Creating a PMM file" because not all platforms will
store PMM data in a file.  However, PMM creation takes place on the desktop
using a converter tool that takes a Python source file as input and produces a
``.pmm`` file as output.  So PMM data can be thought of as a file and I use the
two terms interchangably.

The converter tool ``src/tools/pmPyToPmm.py`` is fairly short and easy to
comprehend.  The tool runs on the command line and accepts one python source
file as its one optional argument.  If a filename is given as an argument, a
file with the same base name and ``.pmm`` extension is created as output.
If no argument is given, the tool expects input via ``stdin`` and produces
output on ``stdout``.

The converter tool only does two things: compiles the given Python source to a
code object and marshal's the code object to a byte string.  It is important to
notice that the marshal tool being used is P14p's own ``pmMarshal``.
``pmMarshal`` is conceptually comparable to Python's ``marshal``, but has syntax
differences that make the two marshal formats incompatible.  This
incompatibility was necessary because P14p's code objects are significantly
different than CPython's.

With the ``.pmm`` file created, you must copy the *exact* binary data from the
``.pmm`` file into the platform's storage area.  How this is accomplished is
specific to the platform and storage medium.

IMPORTANT: There is nothing in the ``.pmm`` data that indicates the name of the
module.  So the platform-porting programmer must create a way to find the
``.pmm`` data (and its length in bytes!) by name.  I mention the length because
the ``marshal_load()`` requires the length of the data bytes as an argument.  In
the posix64 example, I use ``fseek()`` and ``ftell()`` to discover the size of
the ``.pmm`` file.
