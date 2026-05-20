#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autocomplete.h"

// Liste de commandes disponibles
static const char *builtin_commands[] = {
	"echo",
	"cd",
	"pwd",
	"ls",
	"mkdir",
	"rm",
	"rmdir",
	"cat",
	"grep",
	"find",
	"cp",
	"mv",
	"touch",
	"exit",
	"history",
	"export",
	"unset",
	"env",
	NULL
};

void suggest_commands(const char *prefix)
{
	int prefix_len = strlen(prefix);
	int found = 0;

	printf("\n");
	for (int i = 0; builtin_commands[i] != NULL; i++)
	{
		if (strncmp(builtin_commands[i], prefix, prefix_len) == 0)
		{
			printf("%s  ", builtin_commands[i]);
			found++;
		}
	}
	if (found > 0)
		printf("\n");
}

char *autocomplete_command(const char *prefix)
{
	int prefix_len = strlen(prefix);
	int count = 0;
	char *match = NULL;

	for (int i = 0; builtin_commands[i] != NULL; i++)
	{
		if (strncmp(builtin_commands[i], prefix, prefix_len) == 0)
		{
			match = (char *)builtin_commands[i];
			count++;
		}
	}

	// Si un seul match, retourner la commande complète
	if (count == 1)
		return match;
	
	// Si plusieurs matches, afficher les suggestions
	if (count > 1)
		suggest_commands(prefix);
	
	return NULL;
}
