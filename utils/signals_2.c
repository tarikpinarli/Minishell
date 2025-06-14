/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 14:55:31 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 14:56:08 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
* The function assigned to the function pointer "rl_event_hook", which is
* defined by the Readline library. It is called periodically (typically several
* times per second) by Readline while it is waiting for terminal input.
* This function allows the Minishell program to check if SIGINT has been
* intercepted, thanks to the SIGINT handler which assigns the value of that
* signal to the global variable g_signal_status. Minishell can then redisplay
* the shell's prompt and set the correct exit code, just like bash does, if
* SIGINT is intercepted during readline.
*/
int	readline_signal_hook(void)
{
	if (g_signal_status == SIGINT)
	{
		(void)last_exit_code(1, 128 + g_signal_status);
		g_signal_status = 0;
		rl_replace_line("", 0);
		(void)write(1, "\n", 1);
		rl_on_new_line();
		rl_redisplay();
	}
	return (0);
}

int	heredoc_signal_hook(void)
{
	if (g_signal_status == SIGINT)
	{
		(void)last_exit_code(1, 128 + g_signal_status);
		rl_done = 1;
	}
	return (0);
}
