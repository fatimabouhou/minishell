#include <stdio.h>

#include "parser.h"
#include "signals.h"
#include "executor.h"
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
        ** Lire la ligne
        */
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        /*
        ** Enlever le \n
        */
        remove_newline(line);

        /*
        ** Ignorer ligne vide
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
        ** Exécution
        */
        execute_command(cmd);

        /*
        ** Libération mémoire
        */
        free_command(cmd);
    }

    return 0;
}