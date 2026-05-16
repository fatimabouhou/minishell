#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "executor.h"
#include "builtins.h"

/*
** Gérer < > >>
*/
void handle_redirections(Command *cmd)
{
    if (cmd->input_file)
    {
        int fd = open(cmd->input_file, O_RDONLY);

        if (fd < 0)
        {
            perror("input");
            exit(EXIT_FAILURE);
        }

        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (cmd->output_file)
    {
        int flags = O_WRONLY | O_CREAT;

        if (cmd->append)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;

        int fd = open(cmd->output_file, flags, 0644);

        if (fd < 0)
        {
            perror("output");
            exit(EXIT_FAILURE);
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

/*
** Exécution simple
*/
void execute_command(Command *cmd)
{
    pid_t pid;

    /*
    ** Commande vide
    */
    if (cmd == NULL || cmd->args[0] == NULL)
        return;

    /*
    ** Built-in
    */
    if (is_builtin(cmd))
    {
        execute_builtin(cmd);
        return;
    }

    /*
    ** Processus fils
    */
    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    /*
    ** Fils
    */
    if (pid == 0)
    {
        handle_redirections(cmd);

        execvp(cmd->args[0], cmd->args);

        fprintf(stderr,
                "MyShell: commande inconnue : %s\n",
                cmd->args[0]);

        exit(EXIT_FAILURE);
    }

    /*
    ** Père
    */
    if (cmd->background)
    {
        printf("[PID] %d\n", pid);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
}