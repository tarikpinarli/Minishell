/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:25 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/07 15:41:50 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_n_flag(char *arg)
{
	int	j;

	if (!arg || ft_strncmp(arg, "-n", 2) != 0)
		return (0);
	j = 2;
	while (arg[j])
	{
		if (arg[j] != 'n')
			return (0);
		j++;
	}
	return (1);
}

static int	skip_n_flags(char **argv, int *next_line)
{
	int	i;

	i = 1;
	*next_line = 1;
	while (argv[i] && is_valid_n_flag(argv[i]))
	{
		*next_line = 0;
		i++;
	}
	return (i);
}

static void	print_arguments(char **argv, int start_index)
{
	int	i;

	i = start_index;
	while (argv[i])
	{
		printf("%s", argv[i]);
		if (argv[i + 1])
			printf(" ");
		i++;
	}
}

int	builtin_echo(char **argv)
{
	int	i;
	int	next_line;

	i = skip_n_flags(argv, &next_line);
	print_arguments(argv, i);
	if (next_line)
		printf("\n");
	return (0);
}


