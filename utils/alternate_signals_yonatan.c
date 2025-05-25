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
//	(void)last_exit_code(1, 128 + sig); // TODO: Can I also avoid having this line here, make the handling function even more concise and atomic? UPDATE: It is working! But I keep this to be checked on Linux first

	/* WARN: these are now commented out on purpose, because I don't want this to be done from here - plus, it causes the prompt to be displayed twice!
	rl_replace_line("", 0); // clear current line
	write(1, "\n", 1);		// move to next line
	rl_on_new_line();		// prepare readline for new input
	rl_redisplay();			// redraw the prompt
	*/
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
		return (-1); // WARN: it is important to handle failure of sigaction().
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
		return (-1); // WARN: it is important to handle failure of sigaction().
	if (!has_run)
	{
		has_run = 1;
		if (setup_sigint_handler() == -1)
			return (-1);
	}
	return (0);
}


// TODO: the function for rl_event_hook !
// WARN: rl_event_hook can be assigned a function which has no return value and
// does not take in any parameters at all. This means that no malloc'ed memory
// can be freed from within that function, and no flag can be returned from it:
// But I suppose that no memory will be allocated before SIGINT would be
// intercepted from the readline, since readline only operates before Enter is
// pressed by the user?
int	readline_signal_hook(void)
{
	if (g_signal_status == SIGINT)
	{
		(void)last_exit_code(1, 128 + g_signal_status);
		rl_replace_line("", 0);
		(void)write(1, "\n", 1);
		rl_on_new_line();
		rl_redisplay();
		g_signal_status = 0;
	}
	return (0);
}



/* WARN: remove this block when ready with the signals!
 * older version, functioining, but using the process enum and redeclaring the
 * SIGINT every single time...
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

	return (0);
}
*/
