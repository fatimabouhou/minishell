#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"


/*
** Vérifie si la commande est un built-in
*/
int is_builtin(Command *cmd)
{
    if (cmd == NULL || cmd->args[0] == NULL)
        return 0;

    if (strcmp(cmd->args[0], "cd") == 0)
        return 1;

    if (strcmp(cmd->args[0], "exit") == 0)
        return 1;

    return 0;
}


/*
** Exécute les built-ins
*/
void execute_builtin(Command *cmd)
{
    /*
    ** Built-in : exit
    */
    if (strcmp(cmd->args[0], "exit") == 0)
    {
        printf("Exiting shell...\n");
        exit(0);
    }

    /*
    ** Built-in : cd
    */
    if (strcmp(cmd->args[0], "cd") == 0)
    {
        /*
        ** Si aucun argument
        */
        if (cmd->args[1] == NULL)
        {
            printf("cd: missing argument\n");
            return;
        }

        /*
        ** Changer de répertoire
        */
        if (chdir(cmd->args[1]) != 0)
        {
            perror("cd");
        }
    }
}