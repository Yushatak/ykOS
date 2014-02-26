ykOS - Yushatak's personal OS project

All required tools are included and in relative paths accessed by the build.bat file.

Build Notes:
The version of dd used in the batch file is from 
http://www.chrysocome.net/dd

Other versions of dd will not necessarily work, instead blanking out the 
floppy image and leaving only what we write (making most emulators choke 
on it).

All ASM in this project is Intel syntax, and all non-inline ASM is 
compiled with NASM.

The included compiler is an i486-elf cross-compiler, and all output is stripped of ELF 
formatting before use.