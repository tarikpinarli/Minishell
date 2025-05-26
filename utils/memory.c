/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:48 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 14:13:57 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_deprecated_strings(t_token *tokens, size_t k)
{
	while (tokens[k].str)
	{
		free(tokens[k].str);
		tokens[k].str = NULL;
		k++;
	}
}

void	cleanup_heredocs(t_command *cmd)
{
	t_redir	*in;
	char	*heredoc_file_name;
	char	*heredoc_number;
	int		i;

	in = cmd->in_redir;
	i = 1;
	while (in)
	{
		if (in->type == REDIR_HEREDOC)
		{
			heredoc_number = ft_itoa(i);
			heredoc_file_name = ft_strjoin("heredoc_", heredoc_number);
			unlink(heredoc_file_name);
			free(heredoc_file_name);
			free(heredoc_number);
			i++;
		}
		in = in->next;
	}
}

void	free_all(char *input, t_token *tokens, t_command *cmd)
{
	(void)tokens;
	if (cmd)
		cleanup_heredocs(cmd);
	if (input)
		free_input(input);
	if (cmd)
		free_cmd(cmd);
}
