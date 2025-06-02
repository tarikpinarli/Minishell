/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/31 17:22:28 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// builtin_export_4.c

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

// builtin_export_3.c
static int	match_env_var(char *arg, char **env, int i, char *eq)
{
	char	*eq_2;

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
	return (-1);
}

static int	match_env_key_only(char *head, char **env)
{
	int		i;
	char	*eq_2;

	i = 0;
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
	return (-1);
}

int	var_exist(char *arg, char **env)
{
	char	*eq;

	eq = strchr(arg, '=');
	if (eq)
	{
		*eq = '\0';
		return (match_env_var(arg, env, 0, eq));
	}
	else
		return (match_env_key_only(arg, env));
}

// builtin_export_2.c
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

static void	sort_env_copy(char **copy)
{
	int		i;
	int		j;
	char	*temp;

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
}

void	sort_and_print_env(char **env)
{
	char	**copy;

	copy = copy_env(env);
	if (!copy)
		return ;
	sort_env_copy(copy);
	print_env(copy);
}

// builtin_export.c
int	remove_env_var(char ***env, int index)
{
	int		i;
	int		j;
	int		count;
	char	**new_env;

	count = 0;
	while ((*env)[count])
		count++;
	new_env = malloc(sizeof(char *) * count);
	if (!new_env)
	{
		write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) -1);
		exit(1);
	}
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

char	**append_env_var(char *new_var, char **env)
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
	{
		write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) -1);
		exit(1);
	}
	while (j < i)
	{
		new_env[j] = ft_strdup(env[j]);
		j++;
	}
	new_env[i] = ft_strdup(new_var);
	new_env[i + 1] = NULL;
	free_two_dimensional_array(&env);
	return (new_env);
}

int	builtin_export(char **argv, int pid_flag, char ***env)
{
	int	i;
	int	exist_index;
	char	**temp;

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
			if (!remove_env_var(env, exist_index))
				return(1);
			temp = append_env_var(argv[i], *env);
			if (!temp)
				return(1);
			else
			{
				*env = temp;
				free_two_dimensional_array(&temp);
			}
		}
		else
		{
			temp = append_env_var(argv[i], *env);
			if (temp)
				*env = temp;
			else
				return(free_two_dimensional_array(&temp), 1);
		}
		i++;
	}
	return (0);
}
