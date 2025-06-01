/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 10:09:09 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/01 13:21:51 by ykadosh          ###   ########.fr       */
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
	env = copy_env(envp);
	while (1)
	{
		if (cmd)
			printf("WE MIGHT HAVE A PROBLEM, \'CMD\' IS NOT NULL AT THE START OF THE LOOP!\n\n"); // just debugging
		if (setup_signal_handling(1) == -1)
		{
			perror("sigaction");
			continue ;
		}
		/* Trying to set these outside of main, since those HAVE TO BE NULL by
		 * this point, otherwise our program is not safe and there would still
		 * be accessible memory!
		input = NULL;
		tokens = NULL;
		cmd = NULL;
		*/
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
		if (!tokenize(input, &tokens)) // it returns 0 if a quotation mark was left unclosed or if no tokens were counted (whitespace input)
			continue ;
		expand_tokens(tokens, input);
		merge_tokens(tokens, input);

		cmd = parse_tokens(tokens, input);
		if (!cmd)
			continue ;
		free_tokens_and_input(&tokens, &input);

		// WARN: just debugging for Yonatan
		if (tokens || input) // just debugging. If this ends up displaying on the screen, I need to review my pointers in the parsing...
			printf("tokens and/or input seem to not be NULL after free_tokens_and_input() call from main()!!\n\n");

		// debugging:
//		print_command_list(cmd);

		if (cmd && cmd->next) // If there is a pipe, cmd->next exists
			execute_pipeline(cmd, &env);
		else if (cmd) // If it's a single command
			(void)exec_command(cmd, &env);
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
		free_cmd(&cmd);
	}
	free_two_dimensional_array(&env);
	rl_clear_history();
	return (0);
}
