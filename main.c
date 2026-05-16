#include <stdio.h>

#include "parser.h"
#include "builtins.h"
#include "signals.h"
#include "utils.h"

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
        print_prompt();

        /*
        ** Lire la commande
        */
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        /*
        ** Enlever le \n
        */
        remove_newline(line);

        /*
        ** Ligne vide
        */
        if (line[0] == '\0')
            continue;

        /*
        ** Vérification syntaxique
        */
        if (check_syntax(line) != 0)
            continue;

        /*
        ** Parsing
        */
        cmd = parse_input(line);

        if (cmd == NULL)
            continue;

        /*
        ** Built-in ?
        */
        if (is_builtin(cmd))
        {
            execute_builtin(cmd);
        }
        else
        {
            printf("Commande externe detectee (executor pas encore implemente)\n");
        }

        /*
        ** Libération mémoire
        */
        free_command(cmd);
    }

    return 0;
}