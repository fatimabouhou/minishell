#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"

#define BUFFER_SIZE 2048

char *expand_variables(const char *str)
{
	static char result[BUFFER_SIZE];
	char var_name[256];
	char *var_value;
	int i = 0;
	int j = 0;
	int k;

	if (str == NULL)
		return NULL;

	memset(result, 0, BUFFER_SIZE);
	
	while (str[i] && j < BUFFER_SIZE - 1)
	{
		if (str[i] == '$' && str[i + 1] && str[i + 1] != ' ')
		{
			i++;
			k = 0;
			// Extraire le nom de la variable
			while (k < 255 && ((str[i] >= 'a' && str[i] <= 'z') ||
				(str[i] >= 'A' && str[i] <= 'Z') ||
				(str[i] >= '0' && str[i] <= '9') ||
				str[i] == '_'))
			{
				var_name[k++] = str[i++];
			}
			var_name[k] = '\0';

			// Récupérer la valeur de la variable
			var_value = getenv(var_name);
			
			if (var_value != NULL)
			{
				// Copier la valeur dans le résultat
				for (int m = 0; var_value[m] && j < BUFFER_SIZE - 1; m++)
					result[j++] = var_value[m];
			}
			// Si la variable n'existe pas, on ne copie rien (elle disparaît)
		}
		else
		{
			result[j++] = str[i++];
		}
	}
	result[j] = '\0';
	return result;
}
