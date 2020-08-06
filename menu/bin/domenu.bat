@echo off
rem This file can be edited to suit your needs. There are two restrictions:
rem   1.  The name of the 't' environment variable should not be changed. The
rem       _domenu_.exe program expects it to exist.
rem   2.  The value of the 't' environment variable can be set to any path and
rem       file, however, the file extension must not end with the letter 't'.

set t=c:\temp.fil

:loop
if not exist c:\devel\domenu\_domenu_.exe goto errexe

if     "%1"=="" c:\devel\domenu\_domenu_.exe c:\devel\domenu\basic
if not "%1"=="" c:\devel\domenu\_domenu_.exe %1 %2 %3

if errorlevel==2 %t%
if errorlevel==1 goto quit
call %t%
goto loop

:errexe
echo _domenu_.exe not found. Update path in %0.
goto quit

:quit
set t=
