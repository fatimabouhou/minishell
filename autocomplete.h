#ifndef AUTOCOMPLETE_H
# define AUTOCOMPLETE_H

/*
** Display command suggestions based on partial input
** Example: "ec" suggests "echo"
*/
void	suggest_commands(const char *prefix);

/*
** Get the best matching command
** Returns the completed command or NULL if no match
*/
char	*autocomplete_command(const char *prefix);

#endif
