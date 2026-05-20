#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "signals.h"
#include "executor.h"
#include "utils.h"
#include "history.h"
#include "env.h"
#include "autocomplete.h"

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

        // Auto-complétion - si la ligne finit par ?, afficher les suggestions
        if (strlen(line) > 0 && line[strlen(line) - 1] == '?')
        {
            char prefix[1024];
            strncpy(prefix, line, strlen(line) - 1);
            prefix[strlen(line) - 1] = '\0';
            
            // Si c'est juste "?", afficher toutes les commandes
            if (strlen(prefix) == 0)
            {
                printf("Commandes disponibles:\n");
                suggest_commands("");
            }
            else
            {
                printf("Suggestions pour '%s':\n", prefix);
                suggest_commands(prefix);
            }
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

        // Diviser par ; pour les commandes multiples
        char *commands[100];
        int cmd_count = 0;
        char line_copy[1024];
        char *cmd_ptr;
        char *saveptr;

        strcpy(line_copy, line);
        cmd_ptr = strtok_r(line_copy, ";", &saveptr);
        
        while (cmd_ptr != NULL && cmd_count < 100)
        {
            // Supprimer les espaces au début et à la fin
            while (*cmd_ptr == ' ' || *cmd_ptr == '\t')
                cmd_ptr++;
            int len = strlen(cmd_ptr);
            while (len > 0 && (cmd_ptr[len - 1] == ' ' || cmd_ptr[len - 1] == '\t'))
                len--;
            if (len > 0)
            {
                char *trimmed_cmd = malloc(len + 1);
                strncpy(trimmed_cmd, cmd_ptr, len);
                trimmed_cmd[len] = '\0';
                commands[cmd_count++] = trimmed_cmd;
            }
            cmd_ptr = strtok_r(NULL, ";", &saveptr);
        }

        // Exécuter chaque commande
        for (int i = 0; i < cmd_count; i++)
        {
            char *current_cmd = commands[i];

            /*
            ** Vérification syntaxique
            */
            if (check_syntax(current_cmd) != 0)
            {
                free(current_cmd);
                continue;
            }

            /*
            ** Parsing
            */
            cmd = parse_input(current_cmd);

            if (cmd == NULL)
            {
                free(current_cmd);
                continue;
            }

            /*
            ** Exécution
            */
            execute_command(cmd);

            /*
            ** Libération mémoire
            */
            free_command(cmd);
            free(current_cmd);
        }
    }

    return 0;
}