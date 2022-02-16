#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include "blang.h"

int blangParse(std::string cmd_file) 
{
    char* fullpath = realpath(cmd_file.c_str(), NULL);

    // open a file handle to a particular file:
    FILE *fh = fopen(fullpath, "r");
    // make sure it's valid:
    if (!fh) 
    {
        return -1;
    }
    yyread(fh);

    return 0;
}
