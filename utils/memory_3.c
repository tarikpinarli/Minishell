/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 13:42:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 13:43:12 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_2D_char(char **arr)
{
	int i;

	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

void	ft_free_split(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

void	free_env(char **env)
{
	int i = 0;
	while (env[i])
		free(env[i++]);
	free(env);
}



void	free_input(char *input)
{
	if (input)
		free(input);
}

void	free_tokens(t_token *tokens)
{
	int	i;

	i = 0;
	if (tokens)
	{
		while (tokens[i].str != NULL)
		{
			free(tokens[i].str);
			tokens[i].str =  NULL;
			i++;
		}
		free(tokens);
		tokens = NULL;
	}
}
