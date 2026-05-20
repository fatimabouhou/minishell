#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"
#include "utils.h"

#define MAX_HISTORY 100

static char *g_history[MAX_HISTORY];
static int g_history_count = 0;

void init_history(void)
{
    // Potentiellement, charger l'historique depuis un fichier ici
}

void add_to_history(const char *cmd)
{
    if (g_history_count > 0 && strcmp(g_history[g_history_count - 1], cmd) == 0)
    {
        return; // Ne pas ajouter la même commande deux fois de suite
    }

    if (g_history_count < MAX_HISTORY)
    {
        g_history[g_history_count++] = strdup(cmd);
    }
    else
    {
        free(g_history[0]);
        for (int i = 1; i < MAX_HISTORY; i++)
        {
            g_history[i - 1] = g_history[i];
        }
        g_history[MAX_HISTORY - 1] = strdup(cmd);
    }
}

void print_history(void)
{
    for (int i = 0; i < g_history_count; i++)
    {
        printf(" %d  %s\n", i + 1, g_history[i]);
    }
}

char *get_history_entry(int index)
{
    if (index > 0 && index <= g_history_count)
    {
        return g_history[index - 1];
    }
    return NULL;
}
