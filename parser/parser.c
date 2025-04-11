/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:16:33 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 15:50:30 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_args(char **tokens)
{
	int i = 0;
	int count = 0;

	while (tokens[i])
	{
		if (!strcmp(tokens[i], "<") || !strcmp(tokens[i], ">") ||
            !strcmp(tokens[i], ">>") || !strcmp(tokens[i], "|"))
		{
			i += 2; // yönlendirme veya pipe sonrası argüman atlanır
			continue;
		}
		count++;
		i++;
	}
	return count;
}

t_command	*parse_tokens(char **tokens)
{
	t_command	*cmd;
	int			arg_index = 0;
	int			i = 0;

	cmd = ft_calloc(1, sizeof(t_command));
	if (!cmd)
		return (NULL);

	cmd->args = malloc(sizeof(char *) * (count_args(tokens) + 1));
	if (!cmd->args)
		return (free(cmd), NULL);

	while (tokens[i])
	{
		if (!ft_strcmp(tokens[i], ">"))
		{
			cmd->outfile = ft_strdup(tokens[i + 1]);
			cmd->append = 0;
			i += 2;
		}
		else if (!ft_strcmp(tokens[i], ">>"))
		{
			cmd->outfile = ft_strdup(tokens[i + 1]);
			cmd->append = 1;
			i += 2;
		}
		else
		{
			cmd->args[arg_index++] = strip_quotes(tokens[i]);
			i++;
		}
	}
	cmd->args[arg_index] = NULL;
	return (cmd);
}