/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 10:09:09 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/02 18:03:24 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **envp)
{
	char		**env;
	char		*input;
	t_token		*tokens;
	t_command	*cmd;

	(void)argc;
	(void)argv;
	input = NULL;
	tokens = NULL;
	cmd = NULL;
	env = NULL;
	copy_env(envp, &env);
	while (1)
	{
		if (cmd)
			printf("WE MIGHT HAVE A PROBLEM, \'CMD\' IS NOT NULL AT THE START OF THE LOOP!\n\n"); // just debugging
		if (setup_signal_handling(1) == -1)
		{
			perror("sigaction");
			continue ;
		}
		rl_event_hook = &readline_signal_hook;
		input = readline("minishell$ ");
		if (!input)
		{
			printf("exit\n");
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			break ;
		}
		if (input[0])
			add_history(input);
		else  // this is for when the input string is empty - just a null terminator
		{
			free(input);
			input = NULL;
			continue ;
		}
		if (!tokenize(input, &tokens, &env)) // it returns 0 if a quotation mark was left unclosed or if no tokens were counted (whitespace input)
			continue ;
		expand_tokens(tokens, input, &env);

		size_t	i = 0;
		while (tokens[i].str)
		{
			printf("tokens[%zu].quote:	%d	.line_id	%d	.str:	%s\n", i, tokens[i].quote, tokens[i].line_id, tokens[i].str);
			i++;
		}
		merge_tokens(tokens, input, &env);
		cmd = parse_tokens(tokens, input, &env);
		if (!cmd)
			continue ;
		free_tokens_and_input(&tokens, &input);

		if (cmd && cmd->next)
			execute_pipeline(cmd, &env);
		else if (cmd)
			(void)exec_single_command(cmd, &env);
		/*
		 * NOTE: we try to remove this cleanup_heredocs() from here, and always
		 * do it from the execution when necessary, for each node separately.
		if (cmd)
		{
			if (!cleanup_heredocs(cmd))
			{
				free_rest(NULL, &cmd, &env);
				rl_clear_history();
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				exit (last_exit_code(1, 1));
			}
		}
		*/
		free_cmd(&cmd);
	}
	free_two_dimensional_array(&env);
	rl_clear_history();
	return (0);
}
