/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 18:59:42 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/29 20:38:33 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// TODO: check if there are failures in "execute_builtin()"
// that are not caught here right now. UPDATE: there are, malloc failures
// are unprotected there... perhaps other failures, that could be handled there.
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

void	exec_single_cmd_child(t_command *cmd, char **env)
{
	char	*path;

	path = NULL;
	if (!cmd->argv || !cmd->argv[0] || !cmd->argv[0][0])
		exit(0); // WARN: Do we need to free everything from the child here??
//	if (cmd->argv[0][0] == '\0') // earlier version, merged into the tail of the first boolean statement
//		exit(0);
	if (!setup_redirections(cmd))
	{
		free_rest(&path, &cmd, &env);
		exit(1);
	}
	if (cmd->argv[0][0] == '/' || !ft_strncmp(cmd->argv[0], "./", 2)
		|| !ft_strncmp(cmd->argv[0], "../", 3))
		path = ft_strdup(cmd->argv[0]);
	else
		path = find_in_path(env, cmd->argv[0]);
	if (!path)
	{
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
//		free_rest(&path, &cmd, &env); // WARN: let's try again to free everything from the child before exiting without execve().
		exit(127);
	}
	check_if_directory(path, cmd, env);
	if (execve(path, cmd->argv, env) == -1)
		handle_execve_error(cmd->argv[0], path, cmd, env);
	exit(1);
}

// NOTE: The very first cases here need to be tested with the inputs:
// 1. empty input
// 2. '' or "" (empty string)
// 3. << ''
// 4. << ""
// 5. '' << ""
// 6. "" << ""
// 7. echo hi | ""
// 8. echo hello | ''
// 9. echo hello | echo hi | ''

/*
* Return values:
* 1: open(), fork(), waitpid(), sigaction() failure, OR sigint was intercepted
*	 during the here document readline loop.
* 2: command not found (or empty argument/s provided with heredocs)
* 3: returned from child process to communicate a sigaction() failure
* 0: if command was executed properly
*/
int	exec_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	pid_t	wpid;
	int		status;
	int		failure_flag;

	failure_flag = prepare_heredoc_file(cmd);
	if (failure_flag)
	{
		if (failure_flag == -2) // malloc() failed
		{
			cleanup_heredocs(cmd); // WARN: needs check for whether this is necessary...
			free_rest(NULL, &cmd, env);
			write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
			exit (last_exit_code(1, 1));
		}
		else // open() failed OR sigint was intercepted in the heredoc; env() should not be freed - unless we are in the child process!
			return (1);
	}
	printf("HELLO FROM EXEC_COMMAND()!\n\n");
	if (!cmd->argv || !cmd->argv[0] || cmd->argv[0][0] == '\0')
	{
		if (!cmd->in_redir)
		{
			ft_putendl_fd("Command '' not found", 2);
			(void)last_exit_code(1, 127);
		}
		return (2);
	}
	if (is_builtin(cmd->argv[0]))
	{
		exec_isolated_builtin(cmd, env);
		return (0);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		return (1);
	}
	if (pid == 0)
	{
		if (setup_signal_handling(0) == -1)
		{
//			free_rest(NULL, &cmd, env); // WARN: I am not sure at all anymore
			//	regarding freeing the allocated memory in the child!
			return (3); // this return value tells the parent to call perror("sigaction");
		}
		exec_single_cmd_child(cmd, *env); // WARN: this still needs to be checked. Memory (at Hive...) should be freed from the child if execve is not given the power over the program....
	}
	else
	{
		wpid = waitpid(pid, &status, 0);
		if (wpid == -1)
		{
			perror("waitpid");
			return (1);
		}
		if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status) == SIGQUIT)
				write(1, "Quit (core dumped)\n",
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
				last_exit_code(1, 1);
				return (1);
			}
			last_exit_code(1, WEXITSTATUS(status));
		}
		/*
		 * other eventual status monitoring possibilities (might not be necessary for Minishell)
		else if (WIFSTOPPED(status))
		{
			write(1, "Quit (core dumped)\n", (sizeof("Quit (core dumped)\n") - 1));
			last_exit_code(1, 128 + (WSTOPSIG(status)));
		}
		*/
	}
	return (0); // WARN: random return value
}
