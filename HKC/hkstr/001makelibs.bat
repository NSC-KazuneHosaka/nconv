gcc -c hkstr.c -L..\hkmem -o hkstr.o
ar rcs libhkstr.a hkstr.o ..\hkmem\hkmem.o
gcc -shared hkstr.o ..\hkmem\hkmem.o -o hkstr.dll
PAUSE