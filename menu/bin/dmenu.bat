@echo off
set t=c:\temp.fil

if "%1"=="" goto errparam

:loop
if not exist c:\devel\domenu\domenu.exe goto errexe
c:
cd \devel\domenu
domenu.exe %1 %2 %3
if errorlevel==2 %t%
if errorlevel==1 goto quit
call %t%
goto loop


:errparam
echo Please specify a DoMenu menu file.
goto quit

:errexe
echo domenu.exe not found. Update path in %0.
goto quit

:quit
set t=
