#include "../src/mkfs.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    char* img_name = NULL;

    if(argc != 2)
    {
        printf("Usgae: %s <img_name>\n", argv[0]);
        exit(-1);
    }
    img_name = argv[1];

    mkfs(img_name);

    return 0;
}

 