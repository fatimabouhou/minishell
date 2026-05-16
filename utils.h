#ifndef UTILS_H
#define UTILS_H

/*
** Affiche le prompt du shell
*/
void print_prompt(void);

/*
** Supprime le '\n' ajouté par fgets()
*/
void remove_newline(char *str);

#endif