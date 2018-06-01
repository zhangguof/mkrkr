#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdio.h>
#include "tjsCommHead.h"


FILE* wfopen(const wchar_t* filename, const wchar_t* mode);
#define _wfopen wfopen


#endif