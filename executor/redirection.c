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

/*
* return values:
* -1 upon failure of open()
* -2 upon malloc() failure
* 0, otherwise
*/
int	handle_heredoc(t_redir *in_redir, char *delimiter, int i)
{
	char	*file_number;
	char	*file_name;
	char	*line;

	file_number = ft_itoa(file_num);
	if (!file_number)
		return (-2);
	file_name  = ft_strjoin("heredoc_", file_number);
	free(file_number);

	int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0777);
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
	free(in_redir->filename);
	in_redir->filename = file_name;
	return (0);
}



int	prepare_heredoc_file(t_command *cmd, int process_flag)
{
	t_redir *in;
	int		i;
	int		failure_flag;

	
	if (!process_flag)



	if (process_flag)

	failure_flag = 0;
	in = cmd->in_redir;
	i = 1;
	while (in)
	{
		if (in->type == REDIR_HEREDOC)
		{
			failure_flag = handle_heredoc(in, in->filename, i)
			if (failure_flag)
			{
				if (failure_flag == -2) // malloc() failed in handle_heredoc().
					free_cmd(&cmd);
					
				return (0);
			i++;
		}
		in = in->next;
	}
	return (1);
}


int	handle_in_redir(t_command *cmd)
{
	t_redir		*in;
	struct stat	st;
	int			fd;

	in = cmd->in_redir;
	while (in)
	{
		if (stat(in->filename, &st) == -1)
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
			dup2(fd, STDIN_FILENO);
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
		heredoc = prepare_heredoc_file(cmd, 0);
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
