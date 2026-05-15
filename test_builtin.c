#include <stdio.h>
#include <unistd.h>

#include "builtins.h"

int main()
{
    char cwd[1024];
    Command cmd;

    /* Afficher dossier actuel */
    getcwd(cwd, sizeof(cwd));
    printf("Avant cd : %s\n", cwd);

    /* Préparer la commande */
    cmd.args[0] = "exit";
    cmd.args[1] = NULL;

    /* Exécuter */
    if (is_builtin(&cmd))
    {
        execute_builtin(&cmd);
    }

    /* Afficher nouveau dossier */
    getcwd(cwd, sizeof(cwd));
    printf("Apres cd : %s\n", cwd);

    return 0;
}