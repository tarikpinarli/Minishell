/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:43 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/11 20:25:08 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
* return values:
* ◦ count: If everything goes smoothly, this function returns the count returned
*	from count_tokens().
* ◦ -1: if an opening quotation mark has not been closed by the user
* ◦ Setting the exit status to 1, this function frees all heap allocated memory
*	and exits the program if a request for dynamic memory allocation fails.
*/
int	tokenize(char *input, t_token **tokens, t_command *cmd)
{
	int	count;

	count = count_tokens(input);
	if (count == -1)
	{
		free(input);
		return (-1);
	}
	*tokens = ft_calloc((count + 1), sizeof(t_token));
	if (!*tokens)
	{
		free(input);
		exit (last_exit_code(1, 1));
	}
	if (fill_up_tokens(input, tokens, count) == -1)
	{
		free_all(input, *tokens, cmd);
		*tokens = NULL;
		exit (last_exit_code(1, 1));
	}
	return (count);
}
