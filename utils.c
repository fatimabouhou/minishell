#include <stdio.h>
#include <string.h>

#include "utils.h"

/*
** Affiche le prompt
*/
void print_prompt(void)
{
    printf("MyShell> ");
    fflush(stdout);
}

/*
** Supprime le '\n'
*/
void remove_newline(char *str)
{
    if (str == NULL)
        return;

    str[strcspn(str, "\n")] = '\0';
}