@echo off
echo Cleaning Up...
echo --------------
del out\* /f /s /q
del obj\* /f /s /q
rmdir obj
rmdir out
del none
echo -----
echo Done!