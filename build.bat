@echo off

if "%1" == "clean"  goto %1
if "%1" == "domenu" goto %1
if "%1" == "pkg"    goto %1

echo.
echo   Targets:
echo      clean    Clean generated files
echo      domenu   Build _domenu_.exe
echo      pkg      Build FreeDOS package
echo.
goto end


:clean
    rem Clean
    if exist gen\NUL deltree /y gen > NUL
    if exist pkg\NUL deltree /y pkg > NUL
    if exist menu\NUL deltree /y menu > NUL
    if exist doug172\NUL deltree /y doug172 > NUL
    if exist domenu.zip del domenu.zip > NUL
    goto end


:domenu
    rem Build executable
    if exist gen\NUL deltree /y gen > NUL
    if exist menu\NUL deltree /y menu > NUL
    if exist doug172\NUL deltree /y doug172 > NUL
    mkdir gen

    find NUL NUL > NUL               ; Set errorlevel to 1
    unzip doug172.zip
    if errorlevel 1 goto errbuild

    del doug172\bin\dougmenu.exe
    move doug172 menu

    find NUL NUL > NUL               ; Set errorlevel to 1
    pat domenu.pat menu
    if errorlevel 1 goto errbuild

    cd menu\src
    find NUL NUL > NUL               ; Set errorlevel to 1
    wcl dougmenu.c /fe=..\..\gen\_domenu_.exe
    if errorlevel 1 goto errbuild
    cd ..\..

    goto end


:pkg
    rem Build FreeDOS package

    rem It would be more correct to set the GRP variable to 'progs', but doing
    rem so causes domenu to be installed in the root directory. Setting GRP
    rem to 'devel' installs domenu in '\devel\domenu'
    rem 
    set PKG=pkg
    set GRP=devel
    set NAM=domenu
    if exist %PKG%\NUL deltree /y %PKG% > NUL
    if exist %NAM%.zip del %NAM%.zip  > NUL

    mkdir %PKG%
    mkdir %PKG%\appinfo
    set LSM=%PKG%\appinfo\%NAM%.lsm
    echo Begin3> %LSM%
    echo Title:          %NAM%>> %LSM%
    echo Version:        0.1>> %LSM%
    echo Entered-date:   2020-07-21>> %LSM%
    echo Description:    A text-UI menu.>> %LSM%
    echo Keywords:       freedos %NAM%>> %LSM%
    echo Author:         >> %LSM%
    echo Maintained-by:  >> %LSM%
    echo Primary-site:   >> %LSM%
    echo Alternate-site: >> %LSM%
    echo Original-site:  >> %LSM%
    echo Platforms:      DOS>> %LSM%
    echo Copying-policy: GPL>> %LSM%
    echo End>> %LSM%

    mkdir %PKG%\%GRP%
    mkdir %PKG%\source
    mkdir %PKG%\source\%NAM%
    if not exist %PKG%\source\%NAM%\NUL goto err1pkg

    copy basic.mnu    %PKG%\source\%NAM%
    copy build.bat    %PKG%\source\%NAM%
    copy domenu.htm   %PKG%\source\%NAM%
    copy domenu.pat   %PKG%\source\%NAM%
    copy doug172.zip  %PKG%\source\%NAM%
    if not exist      %PKG%\source\%NAM%\doug172.zip goto err2pkg


    mkdir %PKG%\%GRP%\%NAM%
    copy .\gen\_domenu_.exe %PKG%\%GRP%\%NAM%
    if not exist %PKG%\%GRP%\%NAM%\_domenu_.exe goto err3pkg

    copy .\domenu.htm %PKG%\%GRP%\%NAM%
    if not exist %PKG%\%GRP%\%NAM%\domenu.htm goto err3pkg

    copy .\basic.mnu %PKG%\%GRP%\%NAM%
    if not exist %PKG%\%GRP%\%NAM%\basic.mnu goto err3pkg

    copy .\menu\bin\domenu.bat %PKG%\%GRP%\%NAM%
    if not exist %PKG%\%GRP%\%NAM%\domenu.bat goto err3pkg

    copy .\menu\bin\dougmenu.doc %PKG%\%GRP%\%NAM%\domenu.doc
    if not exist %PKG%\%GRP%\%NAM%\domenu.doc goto err3pkg

    copy .\menu\bin\sample.mnu %PKG%\%GRP%\%NAM%
    if not exist %PKG%\%GRP%\%NAM%\sample.mnu goto err3pkg

    rem Do NOT create 'links'. Doing so will cause FreeDOS to create
    rem \FDOS\LINKS\DOMENU.COM which will attempt to execute the _domenu_.exe
    rem executable, however _domenu_.exe should be run from a .BAT file.

    cd .\%PKG%
    rem  -9  Max compression
    rem  -r  Recurse into directories
    zip -9 -r ..\domenu.zip *
    if not exist ..\domenu.zip goto err4pkg
    cd ..

    echo.
    echo The domenu.zip package has been created.
    echo.
    echo To install: fdnpkg install domenu.zip
    echo         or: fdnpkg install-wsrc domenu.zip
    echo  To remove: fdnpkg remove domenu

    goto end


:errbuild
    echo Error building.
    goto end

:err1pkg
    echo Error creating directory structure. Building package failed.
    goto end

:err2pkg
    echo Error copying source files. Building package failed. 
    goto end

:err3pkg
    echo Error copying executable files. Building package failed.
    goto end

:err4pkg
    echo Error creating zip file. Building package failed.
    goto end

:end
