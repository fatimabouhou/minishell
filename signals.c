#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "signals.h"

/*
** Gestion de Ctrl + C
*/
void handle_sigint(int sig)
{
    (void)sig;

    printf("\nMyShell> ");
    fflush(stdout);
}

/*
** Nettoyage des processus fils terminés
*/
void handle_sigchld(int sig)
{
    (void)sig;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

/*
** Installation des handlers
*/
void setup_signals(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGCHLD, handle_sigchld);
}