/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/22 18:32:42 by tpinarli         ###   ########.fr       */
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

void	print_export_err(char *str)
{
	ft_putstr_fd("export: `", 2);
	ft_putstr_fd(str, 2);
	ft_putendl_fd("': not a valid identifier", 2);
}
int	valid_identifier(char *str)
{
	char	*eq;
	char	*head;

	head = str;
	eq = ft_strchr(str, '=');
	if (eq)
	{
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
	}
	else
	{
		if (!ft_isalpha(*head) && *head != '_')
		{
			print_export_err(head);
			return (0);
		}
		while (*head)
		{
			if (!ft_isalnum(*head) && *head != '_')
			{
				print_export_err(head);
				return (0);
			}
			head++;
		}
	}
	return (1);
}

int	var_exist(char *arg, char **env)
{
	char	*eq;
	char	*eq_2;
	char	*head;
	int		i;

	head = arg;
	eq = strchr(arg, '=');
	i = 0;
	if (eq)
	{
		*eq = '\0';
		while (env[i])
		{
			eq_2 = ft_strchr(env[i], '=');
			if (eq_2)
			{
				*eq_2 = '\0';
				if (!ft_strcmp(env[i], arg))
				{
					*eq = '=';
					*eq_2 = '=';
					return (i);
				}
				*eq_2 = '=';
			}
			i++;
		}
		*eq = '=';
	}
	else
	{
		while (env[i])
		{
			eq_2 = ft_strchr(env[i], '=');
			if (eq_2)
			{
				*eq_2 = '\0';
				if (!strcmp(env[i], head))
				{
					*eq_2 = '=';
					return (i);	
				}
				*eq_2 = '=';
			}
			i++;
		}
	}
	return (-1);
}

int remove_env_var(char ***env, int index)
{
	int 	i;
	int		j;
	int		count;
	char	**new_env;

	count = 0;
	while ((*env)[count])
		count++;
	new_env = malloc(sizeof(char *) * count);
	if (!new_env)
		return (0);
	i = 0;
	j = 0;
	while (i < count)
	{
		if (i != index)
			new_env[j++] = ft_strdup((*env)[i]);
		free((*env)[i]);
		i++;
	}
	new_env[j] = NULL;
	free(*env);
	*env = new_env;
	return (1);
}


char **append_env_var(char *new_var, char **env)
{
	int		i;
	int		j;
	char	**new_env;

	i = 0;
	j = 0;
	while (env && env[i])
		i++;
	new_env = malloc(sizeof(char *) * (i + 2));
	if (!new_env)
		return (NULL);
	while (j < i)
	{
		new_env[j] = ft_strdup(env[j]);
		j++;
	}
	new_env[i] = ft_strdup(new_var);
	new_env[i + 1] = NULL;
	free_two_dimensional_array(&env);
	return new_env;
}


int	builtin_export(char **argv, int pid_flag, char ***env)
{
	int		i;
	int exist_index;
	(void)pid_flag;

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
		exist_index = var_exist(argv[i], *env);
		if (exist_index >= 0)
		{
			remove_env_var(env, exist_index);
			*env = append_env_var(argv[i], *env);
		}
		else
			*env = append_env_var(argv[i], *env);
		i++;
	}
	return (0);
}
