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
 * Gère une infinité de pipes
 */
void execute_pipe(Command *cmd)
{
    int in_fd = 0; // Au début, l'entrée vient du terminal
    int pipefd[2];
    pid_t pid;
    Command *current = cmd;

    while (current != NULL) {
        // S'il y a une commande après, on crée un pipe
        if (current->next != NULL) {
            if (pipe(pipefd) < 0) {
                perror("MyShell: pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid = fork();
        if (pid < 0) {
            perror("MyShell: fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Processus fils
            // Gère l'entrée venant du pipe précédent
            if (in_fd != 0) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            // Gère la sortie vers le pipe suivant
            if (current->next != NULL) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                close(pipefd[0]); // Ferme le côté lecture inutilisé par cet enfant
            }

            handle_redirections(current);
            execvp(current->args[0], current->args);
            fprintf(stderr, "MyShell: commande inconnue : %s\n", current->args[0]);
            exit(EXIT_FAILURE);
        }

        // Processus parent
        if (in_fd != 0) {
            close(in_fd); // Parent ferme l'entrée précédente
        }
        if (current->next != NULL) {
            close(pipefd[1]); // Parent ferme l'écriture de ce pipe
            in_fd = pipefd[0]; // Sauvegarde la lecture pour le prochain enfant
        }

        current = current->next;
    }

    // Le parent attend la fin de tous les enfants de la chaîne de pipes
    current = cmd;
    while (current != NULL) {
        wait(NULL);
        current = current->next;
    }
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

    /* commande interne (cd, exit, history) */
    if (is_builtin(cmd)) {
        execute_builtin(cmd);
        return;
    }

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