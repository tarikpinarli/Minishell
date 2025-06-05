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
		//ft_putendl_fd("internal dup function has failed.", 2);
		(void)last_exit_code(1, 1);
		return ;
	}
	if (!setup_redirections(cmd))
	{
		if (dup2(saved_stdin, STDIN_FILENO) == -1
			|| dup2(saved_stdout, STDOUT_FILENO) == -1)
		{
			perror("dup");
			//ft_putendl_fd("internal dup function has failed.", 2);
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
		//ft_putendl_fd("internal dup function has failed.", 2);
		(void)last_exit_code(1, 1);
		return ;
	}
	close(saved_stdin);
	close(saved_stdout);
	(void)last_exit_code(1, ret);
//	cleanup_heredocs(cmd); // is this call necessary, since if we return here we just call this function from main after this?
	return ;
}

void	exec_single_cmd_child(t_command **cmd, char ***env)
{
	char		*path;
	t_command	*current;

	path = NULL;
	current = *cmd;
	if (!current->argv || !current->argv[0] || !current->argv[0][0])
	{
		free_rest(&path, cmd, env);
		exit(0); // WARN: Do we need to free everything from the child here??
	}
//	if (current->argv[0][0] == '\0') // earlier version, merged into the tail of the first boolean statement
//		exit(0);
	if (!setup_redirections(current))
	{
		free_rest(&path, cmd, env);
		exit(1);
	}
	if (current->argv[0][0] == '/' || !ft_strncmp(current->argv[0], "./", 2)
		|| !ft_strncmp(current->argv[0], "../", 3))
	{
		path = ft_strdup(current->argv[0]);
		if (!path)
		{
			free_rest(&path, cmd, env);
			exit(12);
		}
	}
	else
	{
		if (find_in_path(*env, current->argv[0], &path) == -1)
		{
			free_rest(&path, cmd, env);
			exit (12);
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
	exit(1);
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
	int		failure_flag;

	failure_flag = prepare_heredoc_file(cmd, env);
	if (failure_flag)
	{
		if (failure_flag == -2) // malloc() failed
		{
			cleanup_heredocs(cmd->in_redir);
			free_rest(NULL, &cmd, env);
			write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
			exit (last_exit_code(1, 1));
		}
		else // open() failed OR sigint was intercepted in the heredoc; env() should not be freed - unless we are in the child process!
			return (1);
	}
	// TODO: this section needs to be reviewed.
	// TODO: put here the REDIRECTIONS, and only execute commands afterwards!
	if (!cmd->argv) // makes sure not to have a segfault later on if we have no arguments in the current cmd list.
	{
		cleanup_heredocs(cmd->in_redir);
		return (0);
	}
	if (cmd->argv[0] && !cmd->argv[0][0]) // this means the first command is an empty string
	{
		ft_putendl_fd("Command '' not found", 2);
		(void)last_exit_code(1, 127);
		cleanup_heredocs(cmd->in_redir);
		return (2);
	}
	if (is_builtin(cmd->argv[0]))
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
		//WARN: this is wrong!!! we are in the child and we need to exit, not return
		if (setup_signal_handling(0) == -1)
		{
			free_rest(NULL, &cmd, env); // WARN: I am not sure at all anymore
			//	regarding freeing the allocated memory in the child!
			return (3); // this return value tells the parent to call perror("sigaction");
		}
		exec_single_cmd_child(&cmd, env); // WARN: this still needs to be checked. Memory (at Hive...) should be freed from the child if execve is not given the power over the program....
	}
	else // WARN: whole section needs reviewing!
	{
		wpid = waitpid(pid, &status, 0); // THIS IS WRONG!!
		if (wpid == -1)
		{
			perror("waitpid"); // WARN: this is WRONG!!!
			cleanup_heredocs(cmd->in_redir);
			return (1);
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
		}
		else
		{
			if (WEXITSTATUS(status) == 3)
			{
				perror("sigaction");
				cleanup_heredocs(cmd->in_redir);
				last_exit_code(1, 1);
				return (1);
			}
			else if (WEXITSTATUS(status) == 12) // WARN: maybe we don't want to do this here but rather from the child....
			{
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				cleanup_heredocs(cmd->in_redir);
				free_cmd(&cmd);
				free_two_dimensional_array(env);
				exit(1);
			}
			last_exit_code(1, WEXITSTATUS(status));
		}
	}
	cleanup_heredocs(cmd->in_redir);
	return (0);
}
