#include "npsconverter.h"
#include <stdio.h>

int main(void) {
    wchar_t input[] = L"input.csv";
    wchar_t output[] = L"output.csv";
    npsconvert(input, output);
    
    return 0;
}