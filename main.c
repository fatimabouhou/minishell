#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "builtins.h"
#include "executor.h"
#include "signals.h"

int main(void)
{
    char line[1024];
    Command *cmd;

    /*
    ** Installer les signaux
    */
    setup_signals();

    /*
    ** Boucle principale du shell
    */
    while (1)
    {
        /*
        ** Afficher le prompt
        */
        printf("MyShell> ");
        fflush(stdout);

        /*
        ** Lire la commande utilisateur
        */
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        /*
        ** Supprimer le \n final
        */
        line[strcspn(line, "\n")] = '\0';

        /*
        ** Si ligne vide
        */
        if (line[0] == '\0')
            continue;

        /*
        ** Vérifier la syntaxe
        */
        if (check_syntax(line) != 0)
            continue;

        /*
        ** Parser la commande
        */
        cmd = parse_input(line);

        if (cmd == NULL)
            continue;

        /*
        ** Si built-in → pas de fork
        */
        if (is_builtin(cmd))
        {
            execute_builtin(cmd);
        }
        /*
        ** Sinon commande externe
        */
        else
        {
            execute_command(cmd);
        }

        /*
        ** Libérer la mémoire
        */
        free_command(cmd);
    }

    return 0;
}