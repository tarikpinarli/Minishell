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

// SIGINT:	Ctrl + c

// TODO: do not hard code the global variable:
// assign it to the signal, or to zero when you need to reset it.
// look into: rl_event_hook macro: I think it will allow your parent process to notice when a signal has been received
// and, do not do more than setting the value inside the handler.
// all this rl_replace_line etc. should not be done here!
void	handle_sigint_parent(int sig)
{
	g_signal_status = sig;
	(void)last_exit_code(1, 128 + sig);  // fixme: shouldn't this be the sum of the exit code and some value  (127, 130) // should it be void?
	/* WARN: these are now commented out on purpose, because I don't want this to be done from here - plus, it causes the prompt to be displayed twice!
	rl_replace_line("", 0); // clear current line
	write(1, "\n", 1);		// move to next line
	rl_on_new_line();		// prepare readline for new input
	rl_redisplay();			// redraw the prompt
	*/
}


void	handle_sigint_child(int sig)
{
	(void)sig;
	g_signal_status = 0;
	/*
	(void)last_exit_code(1, 130);  // fixme: shouldn't this be the sum of the exit code and some value  (127, 130) // should it be void?
	rl_replace_line("", 0); // clear current line
	write(1, "\n", 1);		// move to next line
	rl_on_new_line();		// prepare readline for new input
	rl_redisplay();			// redraw the prompt
	*/
}


/*
* return values: -1 if sigaction() fails, otherwise 0
*/
int	setup_signals(t_process_setup sig_setup)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	ft_bzero(&sa_int, sizeof(sigaction));
	ft_bzero(&sa_quit, sizeof(sigaction));
	(void)sigemptyset(&sa_int.sa_mask);
	(void)sigaddset(&sa_int.sa_mask, SIGINT);
	sa_int.sa_flags = SA_RESTART;

	// sigquit:	ctrl + '\'
	(void)sigemptyset(&sa_quit.sa_mask);
	(void)sigaddset(&sa_quit.sa_mask, SIGQUIT);
	sa_quit.sa_flags = SA_RESTART;
	// SIGINT:	Ctrl + c
//	if (sig_setup == SETUP_START)
//	{
//		sa_int.sa_handler = &handle_sigint_parent;
//		(void)sigemptyset(&sa_int.sa_mask);
//		sa_int.sa_flags = sa_restart;

		// sigquit:	ctrl + '\'
//		(void)sigemptyset(&sa_quit.sa_mask);
//		(void)sigaddset(&sa_quit.sa_mask, sigquit);
//		sa_quit.sa_flags = sa_restart;
//	}
	if (sig_setup == SETUP_PARENT)
	{
		sa_int.sa_handler = &handle_sigint_parent;
		sa_quit.sa_handler = SIG_IGN;
	}
	else
	{
		sa_int.sa_handler = &handle_sigint_child;
		sa_quit.sa_handler = SIG_DFL;
	}

	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		return (-1); // WARN: random value!! but it is important to handle failure of sigaction().
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
		return (-1); // WARN: random value! but it is important to handle failure of sigaction().
	return (0);
}


/*
 * rewrite draft.
int	setup_signals(t_process_setup sig_setup)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	ft_bzero(&sa_int, sizeof(sigaction));
	ft_bzero(&sa_quit, sizeof(sigaction));

	if (sig_setup == SETUP_PARENT)
	{
		sa_int.sa_handler = &handle_sigint_parent;
		(void)sigemptyset(&sa_int.sa_mask);
		sa_int.sa_flags = SA_RESTART;

		
*/
