/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:48 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/20 15:15:43 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_free_split(char **arr)
{
	int	i = 0;

	if (!arr)
		return;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
void	free_cmd(t_command *cmd)
{
	t_redir	*tmp;
	int		i;

	i = 0;
	if (cmd)
	{
		while (cmd->argv && cmd->argv[i])
			free(cmd->argv[i++]);
		free(cmd->argv);
		while (cmd->in_redir)
		{
			tmp = cmd->in_redir;
			cmd->in_redir = tmp->next;
			free(tmp->filename);
			free(tmp);
		}
		while (cmd->out_redir)
		{
			tmp = cmd->out_redir;
			cmd->out_redir = tmp->next;
			free(tmp->filename);
			free(tmp);
		}
		free(cmd);
	}
}
void	free_all(char *input, t_token *tokens, t_command *cmd)
{
	int		i;

	i = 0;
	if (tokens)
	{
		while (tokens[i].str)
			free(tokens[i++].str);
		free(tokens);
	}
	free(input);
	free_cmd(cmd);
}


