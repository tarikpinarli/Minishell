/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:00:56 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 13:22:31 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	builtin_pwd(char **argv)
{
	int		i;
	char	buf[PATH_MAX];
	char	*ptr;

	i = 0;
	if (argv[1])
	{
		if (argv[1][0] == '-')
		{
			while (argv[1][i] == '-')
				i++;
			if (argv[1][i] != '\0')
				return (ft_putstr_fd(argv[0], 2),
					ft_putendl_fd(": -: invalid option", 2), 1);
		}
	}
	ptr = getcwd(buf, PATH_MAX);
	if (ptr)
	{
		printf("%s\n", ptr);
		return (0);
	}
	return (1);
}
