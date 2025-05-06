/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:35 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/04 12:13:57 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_heredoc(char *delimiter)
{
	char	*line;
	int		fd;
	
	fd = open(".heredoc.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd < 0)
		return (-1);
	while (1)
	{
		line = readline("\001\033[1m\002heredoc> \001\033[0m\002");
		if (!line || !ft_strcmp(line, delimiter))
			break;
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	free(line);
	close(fd);
	fd = open(".heredoc.txt", O_RDONLY);
	if (fd < 0)
		return (-1);
	return (fd);
}

int	handle_in_redir(t_command *cmd)
{
	t_redir	*in;
	int		fd;
	int		i;
	char	*failed_array[50];
	
	in = cmd->in_redir;
	i = 0;
	while (in)
	{
		if (in->type == REDIR_IN)
			fd = open(in->filename, O_RDONLY);
		else if (in->type == REDIR_HEREDOC)
			fd = handle_heredoc(in->filename);
		if (fd < 0)
		{
			failed_array[i] = in->filename;
			i++;	
		}
		in = in->next;
	}
	failed_array[i] = NULL;
	if (failed_array[0])
	{
		fd = open(failed_array[0], O_RDONLY);
		if (fd < 0)
			return (perror(failed_array[0]), 0);	
	}
	else
	{
		dup2(fd, STDIN_FILENO);
		close(fd);	
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
		// If it's not the last redir just close it
		if (out->next == NULL)
			dup2(fd, STDOUT_FILENO); // Only last one gets applied
		close(fd);
		out = out->next;
	}
	return (1);
}


int	setup_redirections(t_command *cmd)
{
	int	in_redir_flag = handle_in_redir(cmd);
	if (in_redir_flag == 0)
		return (0);

	int	out_redir_flag = handle_out_redir(cmd);
	if (out_redir_flag == 0)
		return (0);
	return (1);
}

