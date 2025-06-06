/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 18:59:42 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 13:17:21 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	run_here_documents(t_command **cmd, char ***env);

void	exec_isolated_builtin(t_command *cmd, char ***env)
{
	int	saved_stdin;
	int	saved_stdout;
	int	ret;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdin == -1 || saved_stdout == -1)
	{
		perror("dup");
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		// NOTE: Do we need to close the files here? Apparently it is risky to call close() on an already closed fd...
		(void)last_exit_code(1, 1);
		return ;
	}
	if (!setup_redirections(cmd))
	{
		if (dup2(saved_stdin, STDIN_FILENO) == -1
			|| dup2(saved_stdout, STDOUT_FILENO) == -1)
		{
			perror("dup2");
			close(saved_stdin);
			close(saved_stdout);
			(void)last_exit_code(1, 1);
			return ;
		}
		close(saved_stdin);
		close(saved_stdout);
		(void)last_exit_code(1, 1);
		return ;
	}
	ret = execute_builtin(cmd, 1, env);
	if (ret == -1)
	{
		write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) -1);
		free_two_dimensional_array(env);
		free_cmd(&cmd);
		close(saved_stdin);
		close(saved_stdout);
		exit(1);
	}
	if (dup2(saved_stdin, STDIN_FILENO) == -1
		|| dup2(saved_stdout, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(saved_stdin); //WARN: is this safe, should I first check for if they are not -1?
		close(saved_stdout);
		(void)last_exit_code(1, 1);
		return ;
	}
	close(saved_stdin);
	close(saved_stdout);
	(void)last_exit_code(1, ret);
}

void	exec_single_cmd_child(t_command **cmd, char ***env)
{
	char		*path;
	t_command	*current;

	path = NULL;
	current = *cmd;
	if (!setup_redirections(current))
	{
		free_rest(&path, cmd, env);
		exit(1);
	}
	if (!current->argv || !current->argv[0] || !current->argv[0][0])
	{
		free_rest(&path, cmd, env);
		exit(0);
	}
	if (current->argv[0][0] == '/' || !ft_strncmp(current->argv[0], "./", 2)
		|| !ft_strncmp(current->argv[0], "../", 3))
	{
		path = ft_strdup(current->argv[0]);
		if (!path) // malloc failure
		{
			write(2, "memory allocation failure in child process\n",
				sizeof("memory allocation failure in child process\n") - 1);
			free_rest(&path, cmd, env);
			exit(1);
		}
	}
	else
	{
		if (find_in_path(*env, current->argv[0], &path) == -1) // malloc failure
		{
			write(2, "memory allocation failure in child process\n",
				sizeof("memory allocation failure in child process\n") - 1);
			free_rest(&path, cmd, env);
			exit (1);
		}
		if (!path)
		{
			ft_putstr_fd(current->argv[0], 2);
			ft_putendl_fd(": command not found", 2);
			free_rest(&path, cmd, env);
			exit(127);
		}
	}
	check_if_directory(&path, cmd, env);
	if (execve(path, current->argv, *env) == -1)
		handle_execve_error(current->argv[0], path, cmd, env);
	free_rest(&path, cmd, env);
	exit(0);
}

/*
* Return values:
* 1: open(), fork(), waitpid(), sigaction() failure, OR sigint was intercepted
*	 during the here document readline loop.
* 2: command not found (or empty argument/s provided with heredocs)
* 3: returned from child process to communicate a sigaction() failure
* 0: if command was executed properly
*/
int	exec_single_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	pid_t	wpid;
	int		status;

	if (cmd->in_redir)
		if (!run_here_documents(&cmd, env))
			return (1);

	if (cmd->argv && is_builtin(cmd->argv[0]))
	{
		exec_isolated_builtin(cmd, env);
		cleanup_heredocs(cmd->in_redir);
		return (0);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		cleanup_heredocs(cmd->in_redir);
		(void)last_exit_code(1, 1);
		return (1);
	}
	if (pid == 0)
	{
		if (setup_signal_handling(0) == -1)
		{
			free_rest(NULL, &cmd, env);
			perror("sigaction");
			exit (1);
		}
		exec_single_cmd_child(&cmd, env);
	}
	else
	{
		while (1)
		{
			wpid = waitpid(-1, &status, 0);
			if (wpid == -1)
			{
				if (errno != ECHILD)
				{
					perror("waitpid");
					cleanup_heredocs(cmd->in_redir);
					return (1);
				}
			}
			if (WIFSIGNALED(status))
			{
				if (WTERMSIG(status) == SIGQUIT)
					write(2, "Quit (core dumped)\n",
						(sizeof("Quit (core dumped)\n") - 1));
				else if (WTERMSIG(status) == SIGINT)
					write(1, "\n", 1);
				last_exit_code(1, 128 + (WTERMSIG(status)));
				g_signal_status = 0;
				break ;
			}
			else
			{
				if (WEXITSTATUS(status) == 1)
				{
					cleanup_heredocs(cmd->in_redir);
					last_exit_code(1, 1);
					return (1);
				}
				last_exit_code(1, WEXITSTATUS(status));
				break ;
			}
		}
		if (cmd->argv && cmd->argv[0] && !cmd->argv[0][0]) // this means the first command is an empty string
		{
			ft_putendl_fd("Command '' not found", 2);
			(void)last_exit_code(1, 127);
			cleanup_heredocs(cmd->in_redir);
			return (2);
		}
	}
	cleanup_heredocs(cmd->in_redir);
	return (0);
}

/*
* returns 0 in case sigint was intercepted in the heredoc or if open() failed;
* The Minishell loop shall continue in that case, and env() should not be freed.
* On success, returns 1
*/
static int	run_here_documents(t_command **cmd, char ***env)
{
	int		failure_flag;

	failure_flag = prepare_heredoc_files(*cmd, env);
	if (failure_flag)
	{
		if (failure_flag == -2) // malloc() failed
		{
			cleanup_heredocs((*cmd)->in_redir);
			free_rest(NULL, cmd, env);
			rl_clear_history();
			write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
			exit (last_exit_code(1, 1));
		}
		else // open() failed OR sigint was intercepted in the heredoc; loop should continue, env() should not be freed
			return (0);
	}
	return (1);
}
