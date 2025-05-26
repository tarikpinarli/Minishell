/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_4.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 13:00:52 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 13:24:48 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_export_err(char *str)
{
	ft_putstr_fd("export: `", 2);
	ft_putstr_fd(str, 2);
	ft_putendl_fd("': not a valid identifier", 2);
}

static int	is_valid_key(char *key)
{
	if (!ft_isalpha(*key) && *key != '_')
	{
		print_export_err(key);
		return (0);
	}
	while (*key)
	{
		if (!ft_isalnum(*key) && *key != '_')
		{
			print_export_err(key);
			return (0);
		}
		key++;
	}
	return (1);
}

static int	check_with_equal(char *str, char *eq)
{
	char	*head;

	head = str;
	*eq = '\0';
	if (!ft_isalpha(*str) && *str != '_')
	{
		print_export_err(str);
		*eq = '=';
		return (0);
	}
	while (*str)
	{
		if (!ft_isalnum(*str) && *str != '_')
		{
			print_export_err(head);
			*eq = '=';
			return (0);
		}
		str++;
	}
	*eq = '=';
	return (1);
}

int	valid_identifier(char *str)
{
	char	*eq;

	eq = ft_strchr(str, '=');
	if (eq)
		return (check_with_equal(str, eq));
	else
		return (is_valid_key(str));
}
