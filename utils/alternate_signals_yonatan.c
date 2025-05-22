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

void	handle_sigint(int sig)
{
	(void)sig;
	g_signal_status = 130;
	(void)last_exit_code(1, 130);  // FIXME: shouldn't this be the sum of the exit code and some value  (127, 130) // should it be void?
	rl_replace_line("", 0); // Clear current line
	write(1, "\n", 1);		// Move to next line
	rl_on_new_line();		// Prepare readline for new input
	rl_redisplay();			// Redraw the prompt
}

// setting the global variable to 131 has something to do with cases where cat
// or another blocking 'application' is running, and SIGQUIT is received during
// that process... or something like that. Notice that the exit status is set
// to 131 when running cat and sending SIGQUIT!
/*
void	handle_sigquit(int sig)
{
//	(void)sig; we probably do want to do something with the signal: IGNORE it. But how?
	// signal(sig, SIG_IGN); // this is wrong, as the manual says: "The sigaction()
	// function supersedes the signal() function, and should be used in preference.
	// In particular, sigaction() and signal() should not be used in the same
	// process to control the same signal."
	g_signal_status = 131;  // ask Tarik WHY do we set the signal_status to 131. what does this signal_status even do?
	// is this not confusing with the exit status?
}
*/

/*
int	setup_signals(void)
{
	struct sigaction sa_int;
	struct sigaction sa_quit;

	// SIGINT:		Ctrl + c
	ft_bzero(&sa_int, sizeof(sigaction));
	if (sigemptyset(&sa_int.sa_mask) == -1)
		return (-500);  // how does bash interpret those? what does it do on those failures? how do I check that?!
	sa_int.sa_flags = SA_RESTART; // how about SA_SIGINFO? this will make us use the sigaction function instead of the "handler", which is more robust, because it allows to ALSO get the PID of the process sending the signal! That might be crucial in some fork situations!!
	sa_int.sa_handler = &handle_sigint;
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		return (-513); // WARN: random value!! but it is important to handle failure of sigaction().

	// SIGQUIT:	Ctrl + '\'
	ft_bzero(&sa_quit, sizeof(sigaction));
	if (sigemptyset(&sa_quit.sa_mask) == -1)
		return (-500);  // how does bash interpret those? what does it do on those failures? how do I check that?!
	if (sigaddset(&sa_quit.sa_mask, SIGQUIT) == -1)
		return (-501); // WARN: random value! but it is important to handle failure of sigaction().
	sa_quit.sa_flags = SA_RESTART; // how about SA_SIGINFO? this will make us use the sigaction function instead of the "handler", which is more robust, because it allows to ALSO get the PID of the process sending the signal! That might be crucial in some fork situations!!
//	sa_quit.sa_handler = &handle_sigquit;
	

	sa_quit.sa_handler = SIG_IGN;
//	signal(int sig, __sighandler_t handler)
	sigaction(SIGQUIT, &sa_quit, NULL);

	return (0);
}
*/

// setting the global variable to 131 has something to do with cases where cat
// or another blocking 'application' is running, and SIGQUIT is received during
// that process... or something like that. Notice that the exit status is set
// to 131 when running cat and sending SIGQUIT!
/*
void	handle_sigquit(int sig)
{
	g_signal_status = 131;  // ask Tarik WHY do we set the signal_status to 131. what does this signal_status even do?
	// is this not confusing with the exit status?
	
}
*/

// NOTE: if I run 'cat' with no arguments on bash:
//			1. SIGQUIT makes it have core dump, and exit status is 131.
//			2. if I kill that process from another shell (I can find the process
//			ID by running ps -ax), "Terminated" is printed out, the prompt is
//			returned to the user, and exit status is: 143
//			3. SIGINT seems to behave the same way as when not within 'cat'.

/*
void	handle_sigquit(int sig, siginfo_t *info, void *context)
{
	(void)sig; // just for the program to compile when we don't do anything with it...
	(void)context;
	if (!info->si_pid)
	{
		write(1, "Quit (core dumped)\n", sizeof("Quit (core dumped)\n") - 1);
		g_signal_status = 131;
		last_exit_code(1, (128 + last_exit_code(0, 0)));
	}
	else // IGNORE signal
	{
		SIG_IGN;  // not working...

		//signal(sig, SIG_IGN); // WARN: this is wrong, signal() is incompatible
		// with sigaction(), even though it compiles and works sometimes - but
		// never on the first try when running the minishell - AND it also
		// ignores sigquit on all consecutive iterations of sigquit, also in
		// cat's interractive mode
		return ;
	}
}
*/


int	setup_signals(void)
{
	struct sigaction sa_int;
	struct sigaction sa_quit;

	// SIGINT:	Ctrl + c
	ft_bzero(&sa_int, sizeof(sigaction));
	sa_int.sa_handler = &handle_sigint;
	(void)sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		return (-500); // WARN: random value!! but it is important to handle failure of sigaction().

	// SIGQUIT:	Ctrl + '\'
	ft_bzero(&sa_quit, sizeof(sigaction));
	(void)sigemptyset(&sa_quit.sa_mask);
	(void)sigaddset(&sa_quit.sa_mask, SIGQUIT);
//	sa_quit.sa_flags = SA_RESTART;
//	sa_quit.sa_sigaction = &handle_sigquit;
//	sa_quit.sa_flags = SA_SIGINFO | SA_RESTART;
//	sa_quit.sa_sigaction = &handle_sigquit;

	// version which ONLY ignores the signal:
	// (it works: but it doesn't work for cat in interactive mode, where sigquit
	// does a core dump, and returns the prompt
	sa_quit.sa_flags = SA_RESTART;
	sa_quit.sa_handler = SIG_IGN;
	

	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
		return (-500); // WARN: random value! but it is important to handle failure of sigaction().

	return (0);
}
