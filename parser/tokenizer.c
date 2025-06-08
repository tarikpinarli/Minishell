/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:43 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/06 18:59:02 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
* return values:
* ◦ count: If everything goes smoothly, this function returns the count returned
*	from count_tokens().
* ◦ 0: if an opening quotation mark has not been closed by the user, or if
*	no tokens have been found in the input string and count is empty.
* ◦ Setting the exit status to 1, this function frees all heap allocated memory
*	and exits the program if a request for dynamic memory allocation fails.
*/
int	tokenize(char *input, t_token **tokens, char ***env)
{
	int	count;

	count = count_tokens(input);
	if (count == 0 || count == -1)
		return (free(input), 0);
	*tokens = ft_calloc((count + 1), sizeof(t_token));
	if (!*tokens)
	{
		free(input);
		free_two_dimensional_array(env);
		rl_clear_history();
		write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
		exit (last_exit_code(1, 1));
	}
	if (fill_up_tokens(input, tokens, count) == -1)
	{
		free_tokens_and_input(tokens, &input);
		free_two_dimensional_array(env);
		rl_clear_history();
		write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
		exit (last_exit_code(1, 1));
	}
	return (count);
}
