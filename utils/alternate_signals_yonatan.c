/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alternate_signals_yonatan.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:51 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/14 18:27:06 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <signal.h>

volatile sig_atomic_t	g_signal_status = 0;

// TODO: do not hard code the global variable:
// assign it to the signal, or to zero when you need to reset it.
// look into: rl_event_hook macro: I think it will allow your parent process to notice when a signal has been received
// and, do not do more than setting the value inside the handler.
// all this rl_replace_line etc. should not be done here!
void	handle_sigint(int sig)
{
	g_signal_status = sig;
	(void)last_exit_code(1, 128 + sig);
	/* WARN: these are now commented out on purpose, because I don't want this to be done from here - plus, it causes the prompt to be displayed twice!
	rl_replace_line("", 0); // clear current line
	write(1, "\n", 1);		// move to next line
	rl_on_new_line();		// prepare readline for new input
	rl_redisplay();			// redraw the prompt
	*/
}

//  TODO: I suppose we do not need a separate function for the child for SIGINT? but for now it messes up the file descriptors...
//  it does not make any sense however: If I merge them together, file descriptors get closed or something, not allowing me to use cat,
//  but if I put them separately, it does work - but it is basically the SAME function
//  UPDATE: Maybe you do need to have a START setup as well, so that you won't touch the
//  handling of SIGQUIT every time - only SIGINT!
//  because for now you are redoing the setup for it every single time again.

/*
* sets up signal handling for SIGINT (^C) and SIGQUIT (^\);
* return values: -1 if sigaction() fails, otherwise 0
*/
int	setup_signals(t_process process)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	ft_bzero(&sa_int, sizeof(sigaction));
	ft_bzero(&sa_quit, sizeof(sigaction));
	(void)sigemptyset(&sa_int.sa_mask);
	(void)sigaddset(&sa_int.sa_mask, SIGINT);
	sa_int.sa_flags = SA_RESTART;

	(void)sigemptyset(&sa_quit.sa_mask);
	(void)sigaddset(&sa_quit.sa_mask, SIGQUIT);
	sa_quit.sa_flags = SA_RESTART;
	if (process == PARENT)
	{
		sa_int.sa_handler = &handle_sigint;
		sa_quit.sa_handler = SIG_IGN;
	}
	else
	{
		sa_int.sa_handler = &handle_sigint;
		sa_quit.sa_handler = SIG_DFL;
	}

	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		return (-1); // WARN: random value!! but it is important to handle failure of sigaction().
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
		return (-1); // WARN: random value! but it is important to handle failure of sigaction().
//	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
//		return (-1); // WARN: random value! but it is important to handle failure of sigaction().
	return (0);
}
