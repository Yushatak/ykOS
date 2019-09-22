@echo off
rem Clean Script (clean.bat)
rem Part of the ykOS Project
rem Written by E. "Yushatak" S.
rem Copyright Yushatak 2014
rem All Rights Reserved
echo Cleaning Up...
echo --------------
del out\* /f /s /q
del obj\* /f /s /q
rmdir obj
rmdir out
echo -----
echo Done!