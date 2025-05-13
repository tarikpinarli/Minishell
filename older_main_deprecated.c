/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:52 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/28 17:36:35 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int main(void)
{
	char		*input;
	t_token		*tokens;
	t_command	*cmd;

	//rl_catch_signals = 0; // Disabling read_line()s default signal handling. Check if it violates the subject.
	input = NULL;
	tokens = NULL;
	cmd = NULL;
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
		tokens = tokenize(input);
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
	clear_history(); // WARN: this function is not allowed by the subject, should we remove it? is it for running on mac()?
	return 0;
}
