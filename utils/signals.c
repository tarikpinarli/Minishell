/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:51 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/13 14:30:01 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal_status = 0;

void	handle_sigint(int sig)
{
	(void)sig;
	g_signal_status = 130;
	last_exit_code(1, 130);
	rl_replace_line("", 0); // Clear current line
	write(1, "\n", 1);		// Move to next line
	rl_on_new_line();		// Prepare readline for new input
	rl_redisplay();			// Redraw the prompt
}

void	handle_sigquit(int sig)
{
	(void)sig;
	g_signal_status = 131;
}

void	setup_signals(void)
{
	struct sigaction sa_int;
	struct sigaction sa_quit;

	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa_int, NULL);

	sa_quit.sa_handler = handle_sigquit;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = SA_RESTART;
	sigaction(SIGQUIT, &sa_quit, NULL);
}
