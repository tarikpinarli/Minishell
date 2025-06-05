/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 10:09:09 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/05 19:29:37 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*handle_input(int	*loop_control_flag)
{
	char	*input;

	rl_event_hook = &readline_signal_hook;
	input = readline("\001\033[38;5;117m\002minishell$ \001\033[0m\002");
	if (!input)
	{
		printf("exit\n");
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		*loop_control_flag = BREAK;
		return (NULL);
	}
	if (input[0])
		add_history(input);
	else
	{
		free(input);
		input = NULL;
		*loop_control_flag = CONTINUE;
		return (NULL);
	}
	return (input);
}

static void	process_command(char *input, char ***env, int *loop_control_flag)
{
	t_token		*tokens;
	t_command	*cmd;

	tokens = NULL;
	cmd = NULL;
	if (!tokenize(input, &tokens, env))
	{
		*loop_control_flag = CONTINUE;
		return ;
	}
	expand_tokens(tokens, input, env);
	merge_tokens(tokens, input, env);
	cmd = parse_tokens(tokens, input, env);
	free_tokens_and_input(&tokens, &input);
	if (!cmd)
	{
		free_all(&input, &tokens, &cmd);
		*loop_control_flag = CONTINUE;
		return ;
	}
	if (cmd->next)
		execute_pipeline(cmd, env);
	else
		exec_single_command(cmd, env);
	cleaning_after_exec(input, tokens, cmd);
}

int	main(int argc, char **argv, char **envp)
{
	int		loop_control_flag;
	char	**env;
	char	*input;

	env = NULL;
	mute_args(argc, argv);
	copy_env(envp, &env);
	while (1)
	{
		loop_control_flag = RESET;
		setup_signals(&loop_control_flag);
		if (loop_control_flag == CONTINUE)
			continue ;
		input = handle_input(&loop_control_flag);
		if (loop_control_flag == CONTINUE)
			continue ;
		if (loop_control_flag == BREAK)
			break ;
		process_command(input, &env, &loop_control_flag);
		if (loop_control_flag == CONTINUE)
			continue ;
	}
	final_cleaning(env);
	return (0);
}

// int	main(int argc, char **argv, char **envp)
// {
// 	char		**env;
// 	char		*input;
// 	t_token		*tokens;
// 	t_command	*cmd;

// 	(void)argc;
// 	(void)argv;
// 	input = NULL;
// 	tokens = NULL;
// 	cmd = NULL;
// 	env = NULL;
// 	copy_env(envp, &env);
// 	while (1)
// 	{
// 		if (setup_signal_handling(1) == -1)
// 		{
// 			perror("sigaction");
// 			continue ;
// 		}
// 		rl_event_hook = &readline_signal_hook;
// 		input = readline("minishell$ ");
// 		if (!input)
// 		{
// 			printf("exit\n");
// 			close(STDIN_FILENO);
// 			close(STDOUT_FILENO);
// 			close(STDERR_FILENO);
// 			break ;
// 		}
// 		if (input[0])
// 			add_history(input);
// 		else  // this is for when the input string is empty - just a null terminator
// 		{
// 			free(input);
// 			input = NULL;
// 			continue ;
// 		}
// 		if (!tokenize(input, &tokens, &env)) // it returns 0 if a quotation mark was left unclosed or if no tokens were counted (whitespace input)
// 			continue ;
// 		expand_tokens(tokens, input, &env);

// 		size_t	i = 0;
// 		while (tokens[i].str)
// 		{
// 			printf("tokens[%zu].quote:	%d	.line_id	%d	.str:	%s\n", i, tokens[i].quote, tokens[i].line_id, tokens[i].str);
// 			i++;
// 		}
// 		merge_tokens(tokens, input, &env);
// 		cmd = parse_tokens(tokens, input, &env);
// 		if (!cmd)
// 			continue ;
// 		free_tokens_and_input(&tokens, &input);

// 		if (cmd && cmd->next)
// 			execute_pipeline(cmd, &env);
// 		else if (cmd)
// 			(void)exec_single_command(cmd, &env);
// 		free_cmd(&cmd);
// 	}
// 	free_two_dimensional_array(&env);
// 	rl_clear_history();
// 	return (0);
// }
