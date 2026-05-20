#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "signals.h"
#include "executor.h"
#include "utils.h"
#include "history.h"
#include "env.h"

int main(void)
{
    char line[1024];
    Command *cmd;

    /*
    ** Installer les signaux
    */
    setup_signals();
    init_history();

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

        add_to_history(line);

        if (strcmp(line, "history") == 0)
        {
            print_history();
            continue;
        }

        // Ré-exécution d'une commande avec !n
        if (line[0] == '!' && line[1] != '\0')
        {
            int history_index = atoi(&line[1]);
            char *history_cmd = get_history_entry(history_index);
            
            if (history_cmd != NULL)
            {
                printf("%s\n", history_cmd);
                strcpy(line, history_cmd);
            }
            else
            {
                printf("Commande non trouvée dans l'historique\n");
                continue;
            }
        }

        // Expansion des variables d'environnement
        char *expanded_line = expand_variables(line);
        strcpy(line, expanded_line);

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