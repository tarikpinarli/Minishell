/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/16 12:05:45 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

void	execute_pipeline(t_command *cmd)
{
	int		pipefd[2];		//this int array will hold two end of the createdd pipe
	int		prev_fd = -1; // önceki pipe'ın read ucu
	pid_t	pid;
	char	*path;

	while (cmd)
	{
		if (cmd->next && pipe(pipefd) == -1)
		{
			perror("pipe");
			return;
		}

		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			return;
		}

		if (pid == 0)
		{
			// Child process

			if (prev_fd != -1)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (cmd->next)
			{
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}
            if (!setup_redirections(cmd))
    		    exit(1);
			if (cmd->argv[0][0] == '/')
				path = ft_strdup(cmd->argv[0]);
			else
				path = find_in_path(cmd->argv[0]);

			if (!path)
			{
				fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
				exit(127);
			}

			execve(path, cmd->argv, environ);
			perror("execve failed");
			free(path);
			exit(1);
		}

		// Parent process
		if (prev_fd != -1)
			close(prev_fd);
		if (cmd->next)
		{
			close(pipefd[1]);
			prev_fd = pipefd[0];
		}

		cmd = cmd->next;
	}

    int status;
    pid_t wpid;

    while ((wpid = wait(&status)) > 0)
        ;

    last_exit_code(1, WEXITSTATUS(status));

}


void	exec_command(t_command *cmd)
{
	pid_t	pid;
	int		status;
	char	*path;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return;

	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		return;
	}
	if (pid == 0)
	{
		// Çocuk process → önce redir ayarlarını yap
		if (!setup_redirections(cmd))
		    exit(1);

		if (cmd->argv[0][0] == '/')
			path = ft_strdup(cmd->argv[0]);
		else
			path = find_in_path(cmd->argv[0]);

		if (!path)
		{
			fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
			exit(127);
		}

		execve(path, cmd->argv, environ);
		perror("execve failed");
		free(path);
		exit(1);
	}
	else
	{
		waitpid(pid, &status, 0);
		last_exit_code(1, WEXITSTATUS(status));
	}
}


