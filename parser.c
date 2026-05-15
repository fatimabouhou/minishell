#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

/*
 * Vérifie la syntaxe de la ligne
 * Retourne 0 si OK, -1 si erreur
 */
int check_syntax(char *line)
{
    int i = 0;

    /* ignorer les espaces au début */
    while (line[i] == ' ' || line[i] == '\t')
        i++;

    /* pipe en début de ligne */
    if (line[i] == '|') {
        fprintf(stderr, "MyShell: erreur de syntaxe : '|' inattendu\n");
        return -1;
    }

    int len = strlen(line);

    /* aller à la fin en ignorant espaces */
    int j = len - 1;
    while (j > 0 && (line[j] == ' ' || line[j] == '\t'))
        j--;

    /* pipe ou redirection en fin de ligne */
    if (line[j] == '|' || line[j] == '>' || line[j] == '<') {
        fprintf(stderr, "MyShell: erreur de syntaxe : symbole inattendu en fin de ligne\n");
        return -1;
    }

    /* vérifier > et < sans fichier après */
    for (int k = 0; k < len; k++) {
        if (line[k] == '>' || line[k] == '<') {
            int next = k + 1;
            /* sauter >> */
            if (line[k] == '>' && line[k + 1] == '>')
                next = k + 2;
            /* sauter les espaces */
            while (line[next] == ' ' || line[next] == '\t')
                next++;
            /* rien après */
            if (line[next] == '\0' || line[next] == '|' ||
                line[next] == '>'  || line[next] == '<') {
                fprintf(stderr, "MyShell: erreur de syntaxe : fichier manquant après redirection\n");
                return -1;
            }
        }
    }

    return 0;
}

/*
 * Analyse la ligne et retourne une liste chaînée de Command
 * Chaque Command correspond à une commande séparée par un pipe
 */
Command *parse_input(char *line)
{
    /* allouer la première commande */
    Command *head = calloc(1, sizeof(Command));
    if (!head) {
        fprintf(stderr, "MyShell: erreur d'allocation mémoire\n");
        return NULL;
    }
    Command *current = head;

    char buf[1024];
    strncpy(buf, line, 1023);
    buf[1023] = '\0';

    char *token = strtok(buf, " \t");
    int arg_i = 0;

    while (token != NULL) {

        /* redirection sortie append >> */
        if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " \t");
            if (token) {
                current->output_file = strdup(token);
                current->append = 1;
            }

        /* redirection sortie > */
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t");
            if (token) {
                current->output_file = strdup(token);
                current->append = 0;
            }

        /* redirection entrée < */
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t");
            if (token)
                current->input_file = strdup(token);

        /* exécution en arrière-plan & */
        } else if (strcmp(token, "&") == 0) {
            current->background = 1;

        /* pipe | → nouvelle commande */
        } else if (strcmp(token, "|") == 0) {
            current->args[arg_i] = NULL;
            arg_i = 0;

            Command *next_cmd = calloc(1, sizeof(Command));
            if (!next_cmd) {
                fprintf(stderr, "MyShell: erreur d'allocation mémoire\n");
                free_command(head);
                return NULL;
            }
            current->next = next_cmd;
            current = next_cmd;

        /* argument normal */
        } else {
            if (arg_i < MAX_ARGS - 1) {
                current->args[arg_i++] = strdup(token);
            }
        }

        token = strtok(NULL, " \t");
    }

    /* terminer le tableau args avec NULL */
    current->args[arg_i] = NULL;

    return head;
}

/*
 * Libère toute la mémoire d'une liste chaînée de Command
 */
void free_command(Command *cmd)
{
    while (cmd != NULL) {
        /* libérer les arguments */
        for (int i = 0; i < MAX_ARGS && cmd->args[i] != NULL; i++) {
            free(cmd->args[i]);
            cmd->args[i] = NULL;
        }

        /* libérer les fichiers de redirection */
        if (cmd->input_file) {
            free(cmd->input_file);
            cmd->input_file = NULL;
        }
        if (cmd->output_file) {
            free(cmd->output_file);
            cmd->output_file = NULL;
        }

        /* passer au suivant et libérer le courant */
        Command *tmp = cmd->next;
        free(cmd);
        cmd = tmp;
    }
}