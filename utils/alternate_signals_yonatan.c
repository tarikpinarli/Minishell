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



void	handle_sigquit(int sig)
{
//	(void)sig;
	signal(sig, SIG_IGN);
	g_signal_status = 131;  // ask Tarik WHY do we set the signal_status to 131. what does this signal_status even do?
}



int	setup_signals(void)
{
	struct sigaction sa_int;
	struct sigaction sa_quit;

	// sig_int:		Ctrl + c
	ft_bzero(&sa_int, sizeof(sigaction));
	if (sigemptyset(&sa_int.sa_mask) == -1)
		return (-500);  // how does bash interpret those? what does it do on those failures? how do I check that?!
	sa_int.sa_flags = SA_RESTART; // how about SA_SIGINFO? this will make us use the sigaction function instead of the "handler", which is more robust, because it allows to ALSO get the PID of the process sending the signal! That might be crucial in some fork situations!!
	sa_int.sa_handler = &handle_sigint;
	sigaction(SIGINT, &sa_int, NULL);

	// sig_quit:	Ctrl + '\'
	ft_bzero(&sa_quit, sizeof(sigaction));
	if (sigemptyset(&sa_quit.sa_mask) == -1)
		return (-500);  // how does bash interpret those? what does it do on those failures? how do I check that?!
	if (sigaddset(&sa_quit.sa_mask, SIGQUIT) == -1)
		return (-501); // WARN: random value!
	sa_quit.sa_flags = SA_RESTART; // how about SA_SIGINFO? this will make us use the sigaction function instead of the "handler", which is more robust, because it allows to ALSO get the PID of the process sending the signal! That might be crucial in some fork situations!!
	sa_quit.sa_handler = &handle_sigquit;
	

//	sa_quit.sa_handler = signal(SIGQUIT, SIG_IGN);
//	signal(int sig, __sighandler_t handler)
	sigaction(SIGQUIT, &sa_quit, NULL);

	return (0);
}
