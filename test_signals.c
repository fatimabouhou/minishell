#include <stdio.h>
#include <unistd.h>

#include "signals.h"

int main()
{
    setup_signals();

    while (1)
    {
        printf("MyShell> ");
        fflush(stdout);

        pause();
    }

    return 0;
}