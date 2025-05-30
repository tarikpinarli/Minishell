/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:51 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/28 20:06:07 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_status = 0;

void	handle_sigint(int sig)
{
	g_signal_status = sig;
}

static int	setup_sigint_handler(void)
{
	struct sigaction	sa_int;

	ft_bzero(&sa_int, sizeof(sigaction));
	(void)sigemptyset(&sa_int.sa_mask);
	(void)sigaddset(&sa_int.sa_mask, SIGINT);
	sa_int.sa_flags = SA_RESTART;
	sa_int.sa_handler = &handle_sigint;
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		return (-1);
	return (0);
}

/*
* sets up signal handling for SIGINT (^C) and SIGQUIT (^\);
* return values: -1 if sigaction() fails, otherwise 0
*/
int	setup_signal_handling(uint32_t is_parent)
{
	static uint32_t		has_run;
	struct sigaction	sa_quit;

	ft_bzero(&sa_quit, sizeof(sigaction));
	(void)sigemptyset(&sa_quit.sa_mask);
	(void)sigaddset(&sa_quit.sa_mask, SIGQUIT);
	sa_quit.sa_flags = SA_RESTART;
	if (is_parent)
		sa_quit.sa_handler = SIG_IGN;
	else
		sa_quit.sa_handler = SIG_DFL;
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
		return (-1);
	if (!has_run)
	{
		has_run = 1;
		if (setup_sigint_handler() == -1)
			return (-1);
	}
	return (0);
}

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
