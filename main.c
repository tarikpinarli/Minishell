/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:40:00 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/12 20:55:24 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **env)
{
	char		*input;
	t_token		*tokens;
	t_command	*cmd;

	(void)argc;
	(void)argv;
	rl_catch_signals = 0;
	// FIXME: Disabling read_line()s default signal handling.
	// It violates the subject;
	// but it resolves the issue of SIGQUIT signal Ctrl+\, which is not supposed
	// to do anything, but now it prints that and brings the cursor back to the
	// start of the line... UPDATE: To solve this, we need to use a sigaction
	// with a handler that tells our shell to ignore the signal SIGQUIT (basically
	// to not do anything)
	setup_signals();
	while (1)
	{
		input = NULL;
		tokens = NULL;
		cmd = NULL;
		input = readline("minishell$ ");
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		if (input[0])
			add_history(input);
		else  // this is for when the input string is empty - just a null terminator
		{
			free(input);
			continue ;
		}
		if (tokenize(input, &tokens) == -1) // tokenize() returns -1 if a quotation mark was left unclosed.
			continue ;
		expand_tokens(tokens, input);
		merge_tokens(tokens, input);

		cmd = parse_tokens(tokens);
		if (!cmd)
		{
			free_all(input, tokens, cmd);
			printf("Parsing failed.\n");
			continue ;
		}
		/*
		size_t	i = 0;
		printf("in MAIN, at the very end of the loop, the tokens are:\n");
		while (tokens[i].str)
		{
			printf("tokens[%zu].str:		<%s>\ntokens[%zu].quote:	<%d>\n"
				"tokens[%zu].line_id:	<%d>\nlength of string:	<%zu>\n\n\n",
				i, tokens[i].str, i, tokens[i].quote, i, tokens[i].line_id,
				ft_strlen(tokens[i].str));
			i++;
		}
		printf("\n\n");
		*/

		//if (is_builtin(cmd->arg[0]))
			// if its a builtin command execute.
		//print_command(cmd);
		if (cmd && cmd->next) // If there is pipe cmd->next exists
			execute_pipeline(cmd, &env);
		else if (cmd)
			exec_command(cmd, &env); // If its a single command*/
		free_all(input, tokens, cmd);
	}
	rl_clear_history();
//	clear_history(); // WARN: this function is not allowed by the subject, should we remove it? is it for running on mac()?
	return (0);
}
