gcc -c hkmem.c -o hkmem.o
ar rcs libhkmem.a hkmem.o
gcc -shared hkmem.o -o hkmem.dll
PAUSE