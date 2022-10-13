gcc -c hkfile.c -L..\hkmem -L..\hkstr -o hkfile.o
ar rcs libhkfile.a hkfile.o ..\hkmem\hkmem.o ..\hkstr\hkstr.o
gcc -shared hkfile.o ..\hkmem\hkmem.o ..\hkstr\hkstr.o -o hkfile.dll
PAUSE