/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:35 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/12 18:41:21 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	setup_redirections(t_command *cmd)
{
	t_redir	*in = cmd->in_redir;
	t_redir	*out = cmd->out_redir;
	int		fd;

	while (in)
	{
		if (in->type == REDIR_IN)
			fd = open(in->filename, O_RDONLY);
		else
		{
			// REDIR_HEREDOC sonra eklenecek
			in = in->next;
			continue;
		}

		if (fd < 0)
		{
			perror(in->filename);
			return (0);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
		in = in->next;
	}

	while (out)
	{
		if (out->type == REDIR_OUT)
			fd = open(out->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (out->type == REDIR_APPEND)
			fd = open(out->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
		{
			out = out->next;
			continue;
		}

		if (fd < 0)
		{
			perror(out->filename);
			return (0);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
		out = out->next;
	}
	return (1);
}