/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_cmd_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/24 14:34:28 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_cmd_child_logic(t_command *cmd, char ***env)
{
	char	*path;
	int		ret;

	if (!setup_redirections(cmd))
		exit(1);
	if (is_builtin(cmd->argv[0]))
	{
		ret = execute_builtin(cmd, 0, env);
		exit(ret);
	}
	if (cmd->argv[0][0] == '/' || !ft_strncmp(cmd->argv[0], "./", 2) ||
        !ft_strncmp(cmd->argv[0], "../", 3))
		path = ft_strdup(cmd->argv[0]);
	else
		path = find_in_path(*env, cmd->argv[0]);
	if (!path)
	{
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
		exit(127);
	}
	check_if_directory(path, cmd, *env);
	if (execve(path, cmd->argv, *env) == -1)
		handle_execve_error(cmd->argv[0], path, cmd, *env);
	free(path);
	exit(1);
}

void	launch_child_process(t_command *cmd, int prv_fd, int *p_fd, char ***env)
{
	int	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return ;
	}
	if (pid == 0)
	{
		prepare_child(cmd, prv_fd, p_fd);
		exec_cmd_child_logic(cmd, env);
	}
}

void	execute_pipeline(t_command *cmd, char ***env)
{
	int	pipefd[2];
	int	prev_fd;
	int	*curr_pipefd;

	prev_fd = -1;
	while (cmd)
	{
		if (!prepare_heredoc_file(cmd))
    		return ;
		curr_pipefd = NULL;
		if (cmd->next)
		{
			if (!setup_pipe(pipefd))
				return ;
			curr_pipefd = pipefd;
		}
		launch_child_process(cmd, prev_fd, curr_pipefd, env);
		if (prev_fd != -1)
			close(prev_fd);
		update_prev_fd(cmd, &prev_fd, pipefd);
		cmd = cmd->next;
	}
	wait_for_children();
}
