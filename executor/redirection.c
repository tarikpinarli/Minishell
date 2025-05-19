/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:35 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/19 15:55:18 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_heredoc(t_redir *in_redir, char *delimiter, int	file_num)
{
	char	*file_number;
	char	*file_name;
	char	*line;
	int		fd;

	file_number = ft_itoa(file_num);
	file_name = ft_strjoin("heredoc_", file_number);
	free(file_number);
	fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0777);
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
	if (fd < 0)
	{
		free(file_number);
		free(file_name);
		return (-1);
	}
	close(fd);
	fd = open(file_name, O_RDONLY);
	if (fd < 0)
		return (-1);
	if (file_name)
	{
		free(in_redir->filename);
		in_redir->filename = ft_strdup(file_name);
	}
	free(file_name);
	return (fd);
}


int	prepare_heredoc_file(t_command *cmd)
{
	int	i;
	t_redir *in;
	int		fd;

	in = cmd->in_redir;
	i = 1;
	fd = -1;
	while (in)
	{
		if (in->type == REDIR_HEREDOC)
		{
			fd = handle_heredoc(in, in->filename, i);
			i++;
		}
		in = in->next;
	}
	return (fd);
}

int handle_in_redir(t_command *cmd)
{
	t_redir *in = cmd->in_redir;
	int fd = -1;

	while (in)
	{
		fd = open(in->filename, O_RDONLY);
		if (fd < 0)
		{
			perror(in->filename);
			return (0);
		}
		if (in->next == NULL)
		{
			dup2(fd, STDIN_FILENO);
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
			dup2(fd, STDOUT_FILENO);
		close(fd);
		out = out->next;
	}
	return (1);
}

int	setup_redirections(t_command *cmd, int pipeline_flag)
{
	int	heredoc;
	int	in_redir_flag;
	int	out_redir_flag;

	if (!pipeline_flag)
	{
		heredoc = prepare_heredoc_file(cmd);
		if (heredoc == 0)
			return (0);
	}
	in_redir_flag = handle_in_redir(cmd);
	if (in_redir_flag == 0)
		return (0);
	out_redir_flag = handle_out_redir(cmd);
	if (out_redir_flag == 0)
		return (0);
	return (1);
}
