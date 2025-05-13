/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/12 13:06:54 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_env(char **copy)
{
	int		i;
	char	*eq;

	i = 0;
	while (copy[i])
	{
		eq = ft_strchr(copy[i], '=');
		if (eq)
		{
			*eq = '\0';
			printf("declare -x %s=\"%s\"\n", copy[i], eq + 1);
			*eq = '=';
		}
		else
			printf("declare -x %s\n", copy[i]);
		free(copy[i]);
		i++;
	}
	free(copy);
}

void	sort_and_print_env(char **env)
{
	int		i;
	int		j;
	int		env_count;
	char	**copy;
	char	*temp;

	env_count = 0;
	while (env[env_count])
		env_count++;
	copy = malloc((env_count + 1) * sizeof(char *));
	if (!copy)
		return;
	i = 0;
	while (env[i])
	{
		copy[i] = ft_strdup(env[i]);
		i++;
	}
	copy[i] = NULL;
	i = 0;
	while (copy[i])
	{
		j = i + 1;
		while (copy[j])
		{
			if (ft_strcmp(copy[i], copy[j]) > 0)
			{
				temp = copy[i];
				copy[i] = copy[j];
				copy[j] = temp;
			}
			j++;
		}
		i++;
	}
	print_env(copy);
}
//  Simple valid check
int	valid_identifier(char *str)
{
	char	*eq;
	char	*head;

	head = str;
	eq = ft_strchr(str, '=');
	if (eq)
	{
		*eq = '\0';
		while (*str)
		{
			if (!ft_isalnum(*str) && *str != '_')
			{
				ft_putstr_fd("export: ", 2);
				ft_putstr_fd(str, 2);
				ft_putendl_fd("not a valid identifier", 2);
			}
			str++;
		}
		*eq = '=';
	}
	else
	{
		while (*head)
		{
			if (!ft_isalnum(*head) && *head != '_')
				return (0);
			head++;
		}
	}
	return (1);
}

int	builtin_export(char **argv, int pid_flag, char ***env)
{
	int		i;
	int		j;
	int		env_count;
	int		update_flag;
	char	**copy;
	(void)pid_flag;

	update_flag = 0;
	i = 1;
	if (!argv[1])
	{
		sort_and_print_env(*env);
		return (0);
	}
	while (argv[i])
	{
		if (!valid_identifier(argv[i]))
			return (1);
		env_count = 0;
		while ((*env)[env_count])
		{
			if (ft_strcmp((*env)[env_count], argv[i]))
				update_flag = env_count;
			env_count++;
		}
		copy = malloc(sizeof(char *) * (env_count + 2));
		if (!copy)
			return(1);
		j = 0;
		while ((*env)[j])
		{
			copy[j] = (*env)[j];
			j++;
		}
		copy[j++] = ft_strdup(argv[i]);
		copy[j] = NULL;
		(*env) = copy;
		i++;
	}
	return (0);
}
