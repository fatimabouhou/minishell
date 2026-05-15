#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"

/*
** Vérifie si la commande est un built-in
** Retour :
** 1 -> oui
** 0 -> non
*/
int is_builtin(Command *cmd);

/*
** Exécute un built-in
*/
void execute_builtin(Command *cmd);

#endif