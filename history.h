#ifndef HISTORY_H
# define HISTORY_H

void    init_history(void);
void    add_to_history(const char *cmd);
void    print_history(void);
char    *get_history_entry(int index);

#endif
