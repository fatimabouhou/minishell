#ifndef ENV_H
# define ENV_H

/*
** Expands environment variables in a string
** Example: "$HOME/file" becomes "/home/user/file"
*/
char	*expand_variables(const char *str);

#endif
