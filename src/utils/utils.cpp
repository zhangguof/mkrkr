#include "utils.h"


FILE* wfopen(const wchar_t* filename, const wchar_t* mode)  
{  
    char fn[MAX_PATH];  
    char m[MAX_PATH];  
    wcstombs(fn, filename, MAX_PATH);  
    wcstombs(m, mode, MAX_PATH);  
    return fopen(fn, m);  
} 