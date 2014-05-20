REM Create the pcre_chartables.c source file for this platform.

cd %1..\..\src\pattern\pcre
if exist pcre_chartables.c del pcre_chartables.c
if exist dftables.c cl /w dftables.c
if exist dftables.exe dftables.exe pcre_chartables.c
if exist dftables.exe del dftables.exe
if exist dftables.obj del dftables.obj