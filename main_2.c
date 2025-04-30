/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 19:51:05 by ykadosh           #+#    #+#             */
/*   Updated: 2025/04/30 21:11:09 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"

/*
* return values:
* ◦ 0: if everything ran smoothly
* ◦ 1: upon failure of request for dynamic memory allocation
*/
int main(void)
{
	char		*input;
	t_token		*tokens;
	t_command	*cmd;

	input = NULL;
	tokens = NULL;
	cmd = NULL;
	//rl_catch_signals = 0; // Disabling read_line()s default signal handling. Check if it violates the subject.
	setup_signals();
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			printf("exit\n");
			break;
		}
		if (input[0])
			add_history(input);
		else
		{
			free(input);
			continue;
		}
		if (tokenize(input, &tokens) < 0) // tokenize frees input for us if (count < 0), and nothing else gets allocated in that case.
			continue;
		if (!tokens)
		{
			free(input);
			return (1);
		}
		expand_tokens(tokens);
		tokens = merge_tokens(tokens);
		if (!tokens)
		{
			free(input);
			continue;
		}
		cmd = parse_tokens(tokens);
		if (!cmd)
		{
			free(cmd);
			free(tokens);
			printf("Parcing failed.\n");
			continue;
		}
		int	i = 0;
		while (tokens[i].str)
		{
			printf("token[%d] =	<%s>	---- quote type:	<%d>	---- line_id =	<%d>\n", i, tokens[i].str, tokens[i].quote, tokens[i].line_id);
			printf("token[%d].str len = %ld\n", i, ft_strlen(tokens[i].str));
			i++;
		}
		
		//if (is_builtin(cmd->arg[0]))
			// if its a builtin command execute.
        /*if (cmd && cmd->next) // If there is pipe cmd->next exists
            execute_pipeline(cmd);
        else if (cmd)
            exec_command(cmd); // If its a single command*/
		free_all(input, tokens, cmd);
	}
	rl_clear_history();
//	clear_history(); // WARN: this function is not allowed by the subject, should we remove it? is it for running on mac()?
	return 0;
}
