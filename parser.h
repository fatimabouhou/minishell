#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 100

/*
 * Structure représentant une commande
 * selon les besoins du projet
 */
typedef struct Command {
    
    /* arguments */
    char *args[MAX_ARGS];

    /* redirections */
    char *input_file;      // <
    char *output_file;     // > ou >>

    int append;            // 0 => >   1 => >>

    /* exécution en arrière-plan */
    int background;        // &

    /* pipe vers commande suivante */
    struct Command *next;

} Command;


/*
 * Analyse la ligne saisie par l'utilisateur
 * et retourne la structure correspondante
 */
Command *parse_input(char *line);


/*
 * Libération mémoire
 */
void free_command(Command *cmd);


/*
 * Vérification de la syntaxe
 */
int check_syntax(char *line);

#endif