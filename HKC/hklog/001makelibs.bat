gcc -c hklog.c -o hklog.o
ar rcs libhklog.a hklog.o
gcc -shared hklog.o -o hklog.dll
PAUSE