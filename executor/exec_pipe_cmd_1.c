/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_cmd_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/09 12:20:20 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_cmd_child_logic(t_command *current, t_command **cmd, char ***env)
{
	char	*path;
	int		ret;

	path = NULL;
	if (!setup_redirections(current))
	{
		cleanup_child_process(cmd, &path, env);
		exit(1);
	}
	if (!current->argv)
	{
		cleanup_child_process(cmd, &path, env);
		exit (0);
	}
	// WARN: If you delete from above this control structure the check for "if (!current->argv)", put it back at the start of the next if statement!!!
	if (current->argv[0] && !current->argv[0][0])
	{
		ft_putendl_fd("Command '' not found", 2);
		cleanup_child_process(cmd, &path, env);
		exit (127);
	}
	if (is_builtin(current->argv[0]))
	{
		ret = execute_builtin(current, 1, env);
		if (ret == -1)
		{
			cleanup_child_process(cmd, &path, env);
			exit (1);
		}
		cleanup_child_process(cmd, &path, env);
		exit (ret);
	}
	if (current->argv[0][0] == '/' || !ft_strncmp(current->argv[0], "./", 2)
		|| !ft_strncmp(current->argv[0], "../", 3))
	{
		path = ft_strdup(current->argv[0]);
		if (!path)
		{
			cleanup_child_process(cmd, &path, env);
			exit(1); // malloc() failure exits: always 1
		}
	}
	else if (find_in_path(*env, current->argv[0], &path) == -1)
	{
		cleanup_child_process(cmd, &path, env);
		exit(1); // malloc() failure exits: always 1
	}
	if (!path)
	{
		ft_putstr_fd(current->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
		cleanup_child_process(cmd, &path, env);
		exit(127);
	}
	check_if_directory(&path, cmd, env);
	if (execve(path, current->argv, *env) == -1)
		handle_execve_error(current->argv[0], &path, cmd, env);
	exit(0);
}

int	launch_child_process(t_command *current, t_command **cmd, int prev_fd, int *p_fd, char ***env, pid_t *pid)
{
	*pid = fork();
	if (*pid == -1) // fork() failed
	{
		perror("fork");
		return (1);
	}
	if (*pid == 0)
	{
		if (setup_signal_handling(0) == -1) // sigaction() failed
		{
			perror("sigaction");
			cleanup_child_process(cmd, NULL, env);
			exit (1);
		}
		prepare_child(current, prev_fd, p_fd);
		exec_cmd_child_logic(current, cmd, env);
	}
	return (0);
}

/*
 * WARN: when this function is ready, review the return values
* return values:
* 1: open() or fork() failure, OR SIGINT intercepted during heredoc
* 2: command not found (or empty argument/s provided with heredocs)
* -1: upon failure of waitpid(); In this case, Minishell still waits for all
* children that might still be active and marks the failure with perror().
* 0: if execution went smoothly
*/
int	execute_pipeline(t_command *cmd, char ***env)
{
	int			pipefd[2];
	int			*curr_pipefd;
	int			prev_fd;
	t_command	*current;
	pid_t		pid;
	size_t		n_children;
	int			flag;

	prev_fd = -1;
	curr_pipefd = NULL;
	flag = 0;
	pid = 0;
	current = cmd;
	n_children = 0;
	while (current)
	{
		// current_pipefd = NULL;
		if (current->next)
		{
			flag = setup_pipe(pipefd); // pipe() failed. flag == 0
			if (!flag)
			{
				cleanup_heredocs(cmd);
				break ;
			}
			curr_pipefd = pipefd;
		}
		if (launch_child_process(current, &cmd, prev_fd, curr_pipefd, env, &pid) == -1) // fork() failed, we do not increment the children count
		{
			cleanup_heredocs(cmd);
			return (1);
		}
		if (pid > 0)
		n_children++;

		if (prev_fd != -1) // Question to Tarik: should this be before or after the check for an empty argument?
			close(prev_fd);
		update_prev_fd(current, &prev_fd, pipefd);
		current = current->next;
	}
	return (wait_for_children(pid, n_children, cmd));
}
