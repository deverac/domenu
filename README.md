`DoMenu` A Text-UI menu which supports keyboard commands and mouse.

`domenu.zip` is a [FreeDOS](https://www.freedos.org/) package containing `DoMenu` executables (and sources).

To install: `fdinst install domenu.zip`. To install with sources: `fdnpkg install-wsrc domenu.zip`

Run DoMenu by executing `c:\devel\domenu\domenu.bat`. Alternately, add `c:\devel\domenu` to your PATH, and then run `domenu.bat`.

`domenu.doc` contains the official DoMenu documentation. `domenu.htm` contains an abbreviated hyper-linked version of `domenu.doc`.

`domenu.exe` is targeted for FreeDOS, but will run fine in [DOSBox](https://www.dosbox.com/), however, some paths in the `domenu.bat` and .MNU files will need to be updated to work correctly.

`DoMenu` is a patched version of DougMenu, which allows it to be compiled on
FreeDOS. Novell-related features have been disabled and Ctrl-Break handling has
been modified. The name 'DougMenu' was changed to 'DoMenu' in order to avoid any
confusion with the official DougMenu program and as homage to Do(ug)Menu.

The original source files and documentation for DougMenu v1.72 can be found in
`./support/doug172.zip`.


To build from source, the [difpat](https://github.com/deverac/difpat) package must be installed.

To build on FreeDOS:

    C:\>fdnpkg install-wsrc domenu.zip       Install sources
    C:\>cd \fdos\source\domenu               Change directory
    C:\FDOS\SOURCE\DOMENU>build.bat clean    Remove generated files
    C:\FDOS\SOURCE\DOMENU>build.bat domenu   Build domenu.exe
    C:\FDOS\SOURCE\DOMENU>build.bat pkg      Build FreeDOS package domenu.zip


DoMenu cannot be run or built on Linux, however the patchfile is generated on
Linux. The patchfile and other files are stored in a zip file which can be copied to FreeDOS.

To package DoMenu on Linux:

    ./build.sh clean     Remove d.zip
    ./build.sh zip       Generate patchfile and store files for FreeDOS in d.zip
