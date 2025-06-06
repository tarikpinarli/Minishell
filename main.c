/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 10:09:09 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/06 16:09:58 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*handle_input(int	*loop_control_flag, char ***env)
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
		free_two_dimensional_array(env);
		rl_clear_history();
		exit (last_exit_code(0, 0));
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
	if (!cmd)
	{
		*loop_control_flag = CONTINUE;
		return ;
	}
	free_tokens_and_input(&tokens, &input);
	if (cmd->in_redir)
	{
		if (!handle_heredocs(&cmd, env, cmd))
		{
			*loop_control_flag = CONTINUE;
			return ;
		}
	}
	if (cmd->next)
		execute_pipeline(cmd, env);
	else
		exec_single_command(cmd, env);
	free_cmd(&cmd);
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
		input = handle_input(&loop_control_flag, &env);
		if (loop_control_flag == CONTINUE)
			continue ;
		process_command(input, &env, &loop_control_flag);
//		if (loop_control_flag == CONTINUE)  // WARN: this seems unnecesary to me... am I missing something?
//			continue ;
	}
	final_cleaning(env);
	return (0);
}
