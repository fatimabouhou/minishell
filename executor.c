#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "executor.h"
#include "builtins.h"
#include "utils.h"

/*
 * Gère les redirections < > >>
 * À appeler dans le processus fils avant execvp()
 */
void handle_redirections(Command *cmd)
{
    /* redirection entrée < */
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0) {
            perror("MyShell: open input");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    /* redirection sortie > ou >> */
    if (cmd->output_file) {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->append)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;

        int fd = open(cmd->output_file, flags, 0644);
        if (fd < 0) {
            perror("MyShell: open output");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

/*
 * Exécute une liste de commandes reliées par des pipes
 * Exemple : ls | grep .c | wc -l
 */
void execute_pipe(Command *cmd)
{
    int pipefd[2];

    /* créer le tube */
    if (pipe(pipefd) < 0) {
        perror("MyShell: pipe");
        return;
    }

    /* --- processus fils gauche : écrit dans le tube --- */
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("MyShell: fork");
        return;
    }

    if (pid1 == 0) {
        /* fermer le côté lecture */
        close(pipefd[0]);
        /* rediriger stdout vers le tube */
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        /* gérer les redirections éventuelles de cette commande */
        handle_redirections(cmd);

        execvp(cmd->args[0], cmd->args);
        fprintf(stderr, "MyShell: commande inconnue : %s\n", cmd->args[0]);
        exit(EXIT_FAILURE);
    }

    /* --- processus fils droit : lit depuis le tube --- */
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("MyShell: fork");
        return;
    }

    if (pid2 == 0) {
        /* fermer le côté écriture */
        close(pipefd[1]);
        /* rediriger stdin depuis le tube */
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        /* s'il y a encore un pipe après → récursion */
        if (cmd->next->next != NULL) {
            execute_pipe(cmd->next);
        } else {
            /* gérer les redirections éventuelles */
            handle_redirections(cmd->next);
            execvp(cmd->next->args[0], cmd->next->args);
            fprintf(stderr, "MyShell: commande inconnue : %s\n", cmd->next->args[0]);
        }
        exit(EXIT_FAILURE);
    }

    /* --- processus père : fermer les deux côtés et attendre --- */
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

/*
 * Fonction principale d'exécution
 * Gère : builtins, pipes, redirections, background
 */
void execute_command(Command *cmd)
{
    /* commande vide */
    if (cmd == NULL || cmd->args[0] == NULL)
        return;

    // /* commande interne (cd, exit, history) */
    // if (is_builtin(cmd->args[0])) {
    //     execute_builtin(cmd);
    //     return;
    // }

    /* pipe détecté → exécution avec tubes */
    if (cmd->next != NULL) {
        execute_pipe(cmd);
        return;
    }

    /* commande simple : fork + exec */
    pid_t pid = fork();
    if (pid < 0) {
        perror("MyShell: fork");
        return;
    }

    if (pid == 0) {
        /* processus fils */
        handle_redirections(cmd);
        execvp(cmd->args[0], cmd->args);
        /* si execvp échoue */
        fprintf(stderr, "MyShell: commande inconnue : %s\n", cmd->args[0]);
        exit(EXIT_FAILURE);

    } else {
        /* processus père */
        if (cmd->background) {
            /* arrière-plan : ne pas attendre, afficher le PID */
            printf("[PID] %d\n", pid);
        } else {
            /* attendre la fin du fils */
            waitpid(pid, NULL, 0);
        }
    }
}