/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 17:47:23 by tpinarli         ###   ########.fr       */
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
static int match_env_key(char *key, char **env)
{
	int 	i;
	char	*eq;

	i = 0;
	while (env[i])
	{
		eq = ft_strchr(env[i], '=');
		if (eq)
			*eq = '\0';
		if (ft_strcmp(env[i], key) == 0)
		{
			if (eq)
				*eq = '=';
			return (i);
		}
		if (eq)
			*eq = '=';
		i++;
	}
	return (-1);
}

int var_exist(char *arg, char **env)
{
	int		index;
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (eq)
	{
		*eq = '\0';
		index = match_env_key(arg, env);
		*eq = '=';
		return (index);
	}
	else
		return match_env_key(arg, env);
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
		i++;
	}
}

static void	sort_env(char **copy)
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
	sort_env(env);
	print_env(env);
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
		return (0);
	i = 0;
	j = 0;
	while (i < count)
	{
		if (i != index)
		{
			new_env[j++] = ft_strdup((*env)[i]); // Protected
			if (!new_env[j - 1])
			{
				free_two_dimensional_array(&new_env);
				return (0);
			}
		}
		free((*env)[i]);
		i++;
	}
	new_env[j] = NULL;
	free(*env);
	*env = new_env;
	return (1);
}
static char	*replace_env_var(char **env, char *new_var, int exist_index)
{
	char	*temp;

	temp = NULL;
	free(env[exist_index]);
	env[exist_index] = NULL;
	temp = ft_strdup(new_var);
	return (temp);
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
		return (NULL);
	while (j < i)
	{
		new_env[j] =ft_strdup(env[j]); // Protected
		if (!new_env[j])
		{
			free_two_dimensional_array(&new_env);
			return(NULL);
		}
		j++;
	}
	new_env[i] = ft_strdup(new_var); // Protected
	if (!new_env[i])
	{
		free_two_dimensional_array(&new_env);
		return (NULL);
	}
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
			(*env)[exist_index] = replace_env_var(*env, argv[i], exist_index);
			if (!(*env)[exist_index])
				return (-1);
		}
		else
		{
			temp = append_env_var(argv[i], *env);
			if (temp)
				*env = temp;
			else
				return(-1);
		}
		i++;
	}
	return (0);
}
