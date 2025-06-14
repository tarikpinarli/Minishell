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

static void	handle_dup_two_error(int fd)
{
	close(fd);
	perror("dup2");
	(void)last_exit_code(1, 1);
}

/*
* for 'id':	pass 0 for: in redirection
*			pass 1 for: out redirection
* This affcts whether Minishell assigns the file descriptor to the stdin (in
* case of a redirected input) or to the stdout (in case of a redirected output).
*/
static int	handle_error_and_redirect_file(int fd, t_redir *redir, uint32_t id)
{
	int	flag;

	flag = 0;
	if (fd < 0)
	{
		perror(redir->filename);
		return (0);
	}
	if (redir->next == NULL)
	{
		if (id == 0)
			flag = dup2(fd, STDIN_FILENO);
		else
			flag = dup2(fd, STDOUT_FILENO);
		if (flag == -1)
		{
			handle_dup_two_error(fd);
			return (0);
		}
	}
	return (1);
}

static int	handle_in_redir(t_command *current)
{
	struct stat	st;
	t_redir		*in;
	int			fd;

	in = current->in_redir;
	while (in)
	{
		if (in->type == REDIR_IN && stat(in->filename, &st) == -1)
		{
			perror(in->filename);
			return (0);
		}
		fd = open(in->filename, O_RDONLY);
		if (!handle_error_and_redirect_file(fd, in, 0))
			return (0);
		close(fd);
		in = in->next;
	}
	return (1);
}

static int	handle_out_redir(t_command *current)
{
	t_redir	*out;
	int		fd;

	out = current->out_redir;
	while (out)
	{
		if (out->type == REDIR_OUT)
			fd = open(out->filename, O_WRONLY | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		else if (out->type == REDIR_APPEND)
			fd = open(out->filename, O_WRONLY | O_CREAT | O_APPEND,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (!handle_error_and_redirect_file(fd, out, 1))
			return (0);
		close(fd);
		out = out->next;
	}
	return (1);
}

int	setup_redirections(t_command *current)
{
	if (!handle_in_redir(current))
		return (0);
	if (!handle_out_redir(current))
		return (0);
	return (1);
}
