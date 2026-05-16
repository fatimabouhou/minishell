#include <stdio.h>

#include "parser.h"
#include "builtins.h"
#include "signals.h"
#include "executor.h"
#include "utils.h"
#include "executor.h"

int main(void)
{
    char line[1024];
    Command *cmd;

    /*
    ** Installer la gestion des signaux
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
        ** Lire la commande utilisateur
        */
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        /*
        ** Supprimer le '\n'
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
        ** Transformer la ligne en structure Command
        */
        cmd = parse_input(line);

        if (cmd == NULL)
            continue;

        /*
        ** Exécuter la commande
        ** (built-ins, pipes, redirections... sont gérés dans executor)
        */
        execute_command(cmd);
        if (is_builtin(cmd))
        {
            execute_builtin(cmd);
        }
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