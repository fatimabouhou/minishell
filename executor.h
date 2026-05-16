#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

/*
 * Fonction principale : exécute une commande
 * Gère les builtins, pipes, redirections, background
 */
void execute_command(Command *cmd);

/*
 * Gère les redirections < > >>
 * À appeler dans le processus fils avant execvp()
 */
void handle_redirections(Command *cmd);

/*
 * Gère l'exécution de commandes reliées par des pipes
 * Exemple : ls | grep .c
 */
void execute_pipe(Command *cmd);

#endif