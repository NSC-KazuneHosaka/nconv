#include "npsconverter.h"
#include <stdio.h>
#include <time.h>

int main(void) {
    wchar_t input[] = L"input.csv";
    wchar_t output[] = L"output.csv";
    int writtenBytes = 0;

    time_t start_time, end_time;
    clock_t start_clock, end_clock;

    start_clock = clock();
    start_time = time(NULL);

    writtenBytes = npsconvert(input, output);
    wprintf(L"Output was successfully written on %ls (%u byte).\n", output, writtenBytes);
    
    end_time = time(NULL);
    end_clock = clock();
    printf("Total time: %ld ms\n", (end_time - start_time) * 1000);
    printf("CPU Time : %.1f ms\n", ((double)(end_clock - start_clock) / CLOCKS_PER_SEC) * 1000);

    return 0;
}