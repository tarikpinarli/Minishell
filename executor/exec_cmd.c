/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/15 12:25:50 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <sys/types.h>
#include <sys/wait.h>

void	execute_pipeline(t_command *cmd, char ***env)
{
	char	*path;
	int		pipefd[2];		//this int array will hold two end of the created pipe 0 for stdin 1 for stdout
	int		prev_fd;
	int		pid;
	int 	status;
    int 	wpid;
	int		ret;

	prev_fd = -1; // önceki pipe'ın read ucu
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
			if (is_builtin(cmd->argv[0]))
			{
				ret = execute_builtin(cmd, 0, env);
				exit(ret);
			}
			if (cmd->argv[0][0] == '/')
				path = ft_strdup(cmd->argv[0]);
			else
				path = find_in_path(cmd->argv[0]);

			if (!path)
			{
				ft_putstr_fd(cmd->argv[0], 2);
				ft_putendl_fd(": command not found", 2);
				exit(127);
			}
			execve(path, cmd->argv, *env);
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
		else
			prev_fd = -1;
		cmd = cmd->next;
	}
    while ((wpid = wait(&status)) > 0)
    {
		
	}
	unlink(".heredoc.txt");
    last_exit_code(1, WEXITSTATUS(status));
}

int	exec_isolated_builtin(t_command *cmd, char ***env)
{
	int saved_stdin;
	int saved_stdout;
	int	ret;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (!setup_redirections(cmd))
	{
		dup2(saved_stdin, STDIN_FILENO);
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdin);
		close(saved_stdout);
		last_exit_code(1, 1);
		return (1);
	}
	ret = execute_builtin(cmd, 1, env);
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	last_exit_code(1, ret);
	unlink(".heredoc.txt");
	return (1);
}

void exec_single_cmd_child(t_command *cmd, char **env)
{
	char *path;

	if (!setup_redirections(cmd))
		exit(1);

	if (cmd->argv[0][0] == '/')
		path = ft_strdup(cmd->argv[0]);
	else
		path = find_in_path(cmd->argv[0]);

	if (!path)
	{
		write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
		write(2, ": command not found\n", 21);
		exit(127);
	}
	execve(path, cmd->argv, env);
	perror("execve failed");
	free(path);
	exit(1);
}


void	exec_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return;
	if (is_builtin(cmd->argv[0]) && !cmd->next)
		if (exec_isolated_builtin(cmd, env) == 1)
			return;	
	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		return;
	}
	if (pid == 0)
		exec_single_cmd_child(cmd, (*env));
	else
	{
		waitpid(pid, &status, 0);
		last_exit_code(1, WEXITSTATUS(status));
		unlink(".heredoc.txt");
	}
}
