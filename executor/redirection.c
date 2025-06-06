/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:35 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/24 15:39:52 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_in_redir(t_command *cmd)
{
	struct stat	st;
	t_redir		*in;
	int			fd;

	in = cmd->in_redir;
	while (in)
	{
		if (in->type == REDIR_IN && stat(in->filename, &st) == -1)
		{
			perror(in->filename);
			return (0);
		}
		fd = open(in->filename, O_RDONLY);
		if (fd < 0)
		{
			perror(in->filename);
			return (0);
		}
		if (!in->next)
		{
			if (dup2(fd, STDIN_FILENO) == -1)
			{
				close(fd);
				perror("dup2"); // WARN: check that this is only done from the parent!!!
				// ft_putendl_fd("internal dup function has failed.", 2);
				(void)last_exit_code(1, 1);
				return (0); // WARN: is there a child process involved here?
			}
		}
		close(fd);
		in = in->next;
	}
	return (1);
}

int	handle_out_redir(t_command *cmd)
{
	t_redir	*out;
	int		fd;

	out = cmd->out_redir;
	while (out)
	{
		if (out->type == REDIR_OUT)
			fd = open(out->filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		else if (out->type == REDIR_APPEND)
			fd = open(out->filename, O_WRONLY | O_CREAT | O_APPEND, 0777);
		if (fd < 0)
		{
			perror(out->filename);
			return (0);
		}
		if (out->next == NULL)
		{
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				close(fd);
				perror("dup2"); // WARN: protect failure?
				// ft_putendl_fd("internal dup function has failed.", 2);
				(void)last_exit_code(1, 1);
				return (0);
			}
		}
		close(fd);
		out = out->next;
	}
	return (1);
}

int	setup_redirections(t_command *cmd)
{
	if (!handle_in_redir(cmd))
		return (0);
	if (!handle_out_redir(cmd))
		return (0);
	return (1);
}
