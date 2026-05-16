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

/* Fonction utilitaire pour extraire le prochain token en gérant les guillemets */
static char *get_next_token(char **str_ptr) {
    char *s = *str_ptr;
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '\0') {
        *str_ptr = s;
        return NULL;
    }

    char *token = malloc(strlen(s) + 1);
    int i = 0;
    int in_quote = 0;

    /* Gérer symboles spatiaux s'ils sont collés, ex: foo>bar (optionnel mais robuste) */
    if (!in_quote && s[0] == '>' && s[1] == '>') {
        token[0] = '>'; token[1] = '>'; token[2] = '\0';
        *str_ptr = s + 2;
        return token;
    }
    if (!in_quote && (strchr("<>|&", s[0]) != NULL)) {
        token[0] = s[0]; token[1] = '\0';
        *str_ptr = s + 1;
        return token;
    }

    while (*s) {
        /* Si on n'est pas dans des guillemets et qu'on croise un espace ou un symbole, on s'arrête */
        if (!in_quote && (*s == ' ' || *s == '\t' || strchr("<>|&", *s) != NULL)) {
            break;
        }
        
        if (*s == '"') {
            in_quote = !in_quote; /* On change d'état (entre ou sort des guillemets) */
        } else {
            token[i++] = *s; /* Copie le caractère */
        }
        s++;
    }
    token[i] = '\0';
    *str_ptr = s;
    return token;
}

/*
 * Analyse la ligne et retourne une liste chaînée de Command
 * Chaque Command correspond à une commande séparée par un pipe
 */
Command *parse_input(char *line)
{
    Command *head = calloc(1, sizeof(Command));
    if (!head) {
        fprintf(stderr, "MyShell: erreur d'allocation mémoire\n");
        return NULL;
    }
    Command *current = head;

    char *p = line;
    char *token = get_next_token(&p);
    int arg_i = 0;

    while (token != NULL) {

        if (strcmp(token, ">>") == 0) {
            free(token);
            token = get_next_token(&p);
            if (token) {
                current->output_file = token; // token est déjà alloué avec malloc
                current->append = 1;
            }

        } else if (strcmp(token, ">") == 0) {
            free(token);
            token = get_next_token(&p);
            if (token) {
                current->output_file = token;
                current->append = 0;
            }

        } else if (strcmp(token, "<") == 0) {
            free(token);
            token = get_next_token(&p);
            if (token)
                current->input_file = token;

        } else if (strcmp(token, "&") == 0) {
            free(token);
            current->background = 1;

        } else if (strcmp(token, "|") == 0) {
            free(token);
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

        } else {
            if (arg_i < MAX_ARGS - 1) {
                current->args[arg_i++] = token; // token est déjà alloué par malloc
            } else {
                free(token);
            }
        }

        token = get_next_token(&p);
    }

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