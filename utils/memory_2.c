/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 19:19:22 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/05 19:26:18 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_tokens(t_token **tokens)
{
	int	i;

	i = 0;
	while ((*tokens)[i].str != NULL)
	{
		free((*tokens)[i].str);
		i++;
	}
	free(*tokens);
	*tokens = NULL;
}

void	free_all(char **input, t_token **tokens, t_command **cmd)
{
	if (input && *input)
	{
		free(*input);
		*input = NULL;
	}
	if (tokens && *tokens)
		free_tokens(tokens);
	if (cmd && *cmd)
		free_cmd(cmd);
}

void	free_tokens_and_input(t_token **tokens, char **input)
{
	if (input && *input)
	{
		free(*input);
		*input = NULL;
	}
	if (tokens && *tokens)
		free_tokens(tokens);
}

void	final_cleaning(char **env)
{
	free_two_dimensional_array(&env);
	rl_clear_history();
}
